#!/usr/bin/env python3
"""
dts2adt.py — Patch Android/Linux kernel device-tree source (.dts/.dtsi)
so the old `dtc` fork at <https://github.com/darwin-on-arm/dtc-AppleDeviceTree>
can compile them into an Apple DeviceTree binary.

Why patching is needed
----------------------
The darwin-on-arm `dtc-AppleDeviceTree` fork is based on a very old upstream
dtc (default FDT version 17).  Modern Android / Linux kernel DTS files use a
number of syntactic features that this old fork cannot parse, and they also
contain constructs that the Apple DeviceTree (ADT) flat format cannot
represent faithfully:

  * `/plugin/;`, `/delete-node/`, `/delete-property/`, `/bits/ <N>` —
    not understood by the old lexer.
  * `/__local_fixups__/`, `/__symbols__/`, `/__fixups__/` subtrees —
    Linux overlay artifacts that just waste space in an ADT.
  * property & node names longer than 31 chars — silently corrupt the
    ADT (fixed-size 32-byte name buffer, NUL-terminated → 31 usable).
  * `phandle` + `linux,phandle` declared on the same node — produces
    two distinct properties in the ADT, both pointing at the same value.

What this tool does
-------------------
  1. Optionally runs the C preprocessor (gcc -E / cpp) on the input so
     `#include <dt-bindings/...>` and `#define` macros expand.
  2. Strips unsupported directives.
  3. Removes Linux overlay fixup / symbol subtrees.
  4. Truncates node and property names that exceed 31 chars, with a
     collision-avoidance suffix table so distinct long names don't
     collapse onto each other.
  5. De-duplicates `phandle` / `linux,phandle` pairs.
  6. Optionally invokes `dtc -O dtb -o out.dtb patched.dts` to actually
     produce the Apple DeviceTree binary.

Usage
-----
  python3 dts2adt.py \\
      --include-dir /path/to/kernel/include \\
      --include-dir /path/to/kernel/arch/arm64/boot/dts \\
      --dtc /path/to/dtc-AppleDeviceTree/dtc \\
      -o patched.dts --dtb out.dtb \\
      input.dts

See `--help` for the full option list.

This program is free software released under the same GPL terms as dtc.
"""

from __future__ import annotations

import argparse
import os
import re
import shlex
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Optional, Tuple

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

# Per dtc.h in the darwin-on-arm fork:
#   #define MAX_PROPNAME_LEN  31
#   #define MAX_NODENAME_LEN  31
DEFAULT_MAX_NAME_LEN = 31

# Subtree names that are Linux-overlay artifacts and have no meaning
# in an Apple DeviceTree.
FIXUP_NODE_NAMES = {
    "__local_fixups__",
    "__symbols__",
    "__fixups__",
    "__overlay__",
}

# DTS directive keywords that the old dtc fork's lexer cannot parse.
# We delete these from the patched source.
STRIPPED_DIRECTIVES = {
    "plugin",
    "delete-node",
    "delete-property",
    "iommu-strict",  # rare, but unsupported by old lexer
}

# Match a DTS slash-directive at start of a token, e.g. `/dts-v1/`,
# `/plugin/`, `/memreserve/`, `/delete-node/`.  No `^` anchor — we use
# Pattern.match(string, pos) which starts matching at `pos`.
_DIRECTIVE_RE = re.compile(r"/([a-zA-Z][a-zA-Z0-9_-]*)/")

# Match a node-name token: letters, digits, `,`, `.`, `_`, `+`, `-`, `?`, `#`.
# Per the devicetree spec, node names allow: 0-9 a-z A-Z , . _ + - ? #
_NODE_NAME_CHARS = r"a-zA-Z0-9,._+\-?#"  # note: `#` allowed by spec
_PROP_NAME_CHARS = r"a-zA-Z0-9,._+?#*-"   # property names also allow `*`

_IDENT_RE = re.compile(r"[" + _NODE_NAME_CHARS + r"]+(@[" + _NODE_NAME_CHARS + r"]+)?")
_LABEL_RE = re.compile(r"^([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*")


# ---------------------------------------------------------------------------
# Stats / report
# ---------------------------------------------------------------------------

@dataclass
class PatchStats:
    cpp_invoked: bool = False
    directives_stripped: int = 0
    nodes_deleted: int = 0          # fixup subtrees deleted
    props_deleted: int = 0          # bad/duplicate props dropped
    node_names_truncated: int = 0
    prop_names_truncated: int = 0
    rename_map: dict = field(default_factory=dict)  # original -> truncated

    def summary(self) -> str:
        lines = [
            f"  cpp invoked              : {self.cpp_invoked}",
            f"  directives stripped      : {self.directives_stripped}",
            f"  fixup subtrees deleted   : {self.nodes_deleted}",
            f"  duplicate props dropped  : {self.props_deleted}",
            f"  node names truncated     : {self.node_names_truncated}",
            f"  prop names truncated     : {self.prop_names_truncated}",
        ]
        if self.rename_map:
            lines.append("  rename map (original -> truncated):")
            for k, v in self.rename_map.items():
                lines.append(f"    {k!r:50s} -> {v!r}")
        return "\n".join(lines)


# ---------------------------------------------------------------------------
# DTS scanner
# ---------------------------------------------------------------------------
# We don't try to build a full AST — DTS is intentionally C-like and a
# complete parser is well outside the scope of this patcher.  Instead we
# walk the source character-by-character with a small state machine that
# understands:
#   * line comments  // ...
#   * block comments /* ... */
#   * string literals "..."
#   * cell arrays    <...>
#   * byte arrays    [...]
#   * brace depth    { ... }
# At each "interesting" point (directive, node decl, property decl) we
# call back into a transformer that may rewrite the token in place.

class DTSState:
    """Mutable state for the DTS scanner.

    The scanner itself is a single function `scan_dts(text, callbacks)`
    that walks the input; `callbacks` is an object whose methods get
    called when the scanner hits interesting points.  The methods may
    return replacement text (or None to keep the original).
    """

    def __init__(self, text: str):
        self.text = text
        self.pos = 0
        self.n = len(text)
        self.depth = 0          # brace depth (0 == root level)
        self.out: List[str] = []  # emitted output chunks


class ScanCallbacks:
    """Override these methods to transform the DTS in flight.

    Each method receives the matched source text and returns either
    None (keep as-is) or a replacement string.

    Lifecycle for a node `name { ... };`:
        on_node_open(name, decl_text)   -> rewritten decl text (or None)
        ...scanner emits `{` and descends into body, depth += 1...
        ...props / children inside fire on_property / on_node_open recursively...
        ...scanner hits `}`, depth -= 1...
        on_node_exit()                  -> None (no return value used)
        ...scanner emits optional `;`
    """

    def on_directive(self, name: str, full_text: str, at_root: bool) -> Optional[str]:
        return None

    def on_node_open(self, name: str, full_text: str) -> Optional[str]:
        """Called BEFORE the `{`.  full_text is the `name` (or `label: name`)
        text.  Return a replacement string to rewrite the name."""
        return None

    def on_node_exit(self) -> None:
        """Called AFTER the `}`.  No return value is used."""
        return None

    def on_property(self, name: str, full_text: str) -> Optional[str]:
        return None

    def on_fixup_subtree(self, name: str, full_text: str) -> Optional[str]:
        """Called when a /__local_fixups__/ { ... } block is found at root.

        full_text covers the entire block including the closing brace and
        the trailing semicolon.
        """
        return None


def _skip_trivia(state: DTSState, emit: bool = True) -> str:
    """Advance pos past whitespace and comments, optionally copying them
    verbatim into state.out.

    With `emit=True` (default, used at the top of the scan loop) the
    trivia is appended to state.out so it appears in the patched output.
    With `emit=False` (used for mid-token lookahead, e.g. between a
    label and a name) the trivia is NOT emitted here — the caller will
    capture it as part of the token text via state.text[start:state.pos].

    Returns the trivia text consumed (useful for diagnostics).
    """
    text = state.text
    start = state.pos
    while state.pos < state.n:
        c = text[state.pos]
        if c in " \t\r\n":
            state.pos += 1
        elif c == "/" and state.pos + 1 < state.n and text[state.pos + 1] == "/":
            # line comment
            end = text.find("\n", state.pos)
            if end < 0:
                end = state.n
            state.pos = end
        elif c == "/" and state.pos + 1 < state.n and text[state.pos + 1] == "*":
            # block comment
            end = text.find("*/", state.pos + 2)
            if end < 0:
                end = state.n
            else:
                end += 2
            state.pos = end
        else:
            break
    trivia = text[start:state.pos]
    if emit:
        state.out.append(trivia)
    return trivia


def _read_string(state: DTSState) -> str:
    """Read a "..." string literal starting at pos.  Returns the literal
    including surrounding quotes.  Handles backslash escapes."""
    text = state.text
    assert text[state.pos] == '"'
    start = state.pos
    state.pos += 1
    while state.pos < state.n:
        c = text[state.pos]
        if c == "\\":
            state.pos += 2
            continue
        if c == '"':
            state.pos += 1
            break
        state.pos += 1
    return text[start:state.pos]


def _read_balanced(state: DTSState, open_ch: str, close_ch: str) -> str:
    """Read text from pos up to and including the matching close bracket.
    Handles nesting, strings, and comments inside the bracketed region."""
    text = state.text
    start = state.pos
    assert text[state.pos] == open_ch
    state.pos += 1
    depth = 1
    while state.pos < state.n and depth > 0:
        c = text[state.pos]
        if c == "/" and state.pos + 1 < state.n and text[state.pos + 1] == "/":
            end = text.find("\n", state.pos)
            state.pos = end if end >= 0 else state.n
            continue
        if c == "/" and state.pos + 1 < state.n and text[state.pos + 1] == "*":
            end = text.find("*/", state.pos + 2)
            state.pos = (end + 2) if end >= 0 else state.n
            continue
        if c == '"':
            _read_string(state)
            continue
        if c == open_ch:
            depth += 1
        elif c == close_ch:
            depth -= 1
        state.pos += 1
    return text[start:state.pos]


def _read_until_semicolon(state: DTSState) -> str:
    """Read from pos up to and including the next `;` at the current
    brace/bracket depth.  Respects strings, comments, and `<>`, `[]`
    groups so that semicolons inside cell/byte arrays don't fool us."""
    text = state.text
    start = state.pos
    while state.pos < state.n:
        c = text[state.pos]
        if c == "/" and state.pos + 1 < state.n and text[state.pos + 1] == "/":
            end = text.find("\n", state.pos)
            state.pos = end if end >= 0 else state.n
            continue
        if c == "/" and state.pos + 1 < state.n and text[state.pos + 1] == "*":
            end = text.find("*/", state.pos + 2)
            state.pos = (end + 2) if end >= 0 else state.n
            continue
        if c == '"':
            _read_string(state)
            continue
        if c == "<":
            _read_balanced(state, "<", ">")
            continue
        if c == "[":
            _read_balanced(state, "[", "]")
            continue
        if c == "{":
            # Shouldn't happen at this level (caller handles braces), but
            # be defensive.
            _read_balanced(state, "{", "}")
            continue
        if c == ";":
            state.pos += 1
            break
        state.pos += 1
    return text[start:state.pos]


def scan_dts(text: str, cb: ScanCallbacks) -> str:
    """Walk the DTS source top-to-bottom in a single pass, dispatching
    to callbacks.

    The scanner maintains its own brace-depth counter and treats the
    grammar as:

      file ::= (directive | node | reference | property | comment)*
      directive ::= '/' name '/' [';' | rest-of-line-up-to-';']
      node      ::= [label ':'] name ['@' unit] '{' body '}' ';'
      reference ::= '&' label '{' body '}' ';'
      property  ::= [label ':'] name ['=' value] ';'

    Bodies are scanned inline — when the scanner sees `{` it increments
    depth and keeps walking; when it sees `}` it calls on_node_exit and
    decrements depth.  This is simpler and more robust than recursing
    on the inner text.
    """
    state = DTSState(text)

    while state.pos < state.n:
        _skip_trivia(state)
        if state.pos >= state.n:
            break

        c = state.text[state.pos]

        # --- Closing brace: end of current node -------------------------
        if c == "}":
            state.out.append(c)
            state.pos += 1
            state.depth -= 1
            if state.depth < 0:
                # Unbalanced — be defensive, clamp.
                state.depth = 0
            cb.on_node_exit()
            # Optional trailing `;` after `}`.  We look ahead past
            # trivia; if we find `;`, consume it and the trivia;
            # otherwise rewind so the trivia is processed by the next
            # loop iteration (it'll be re-emitted by _skip_trivia).
            save = state.pos
            trivia_end = state.pos
            while trivia_end < state.n and state.text[trivia_end] in " \t\r\n":
                trivia_end += 1
            if trivia_end < state.n and state.text[trivia_end] == ";":
                state.out.append(state.text[save:trivia_end])
                state.out.append(";")
                state.pos = trivia_end + 1
            # else: leave pos at `save`, let the next iteration handle it.
            continue

        # --- Slash directive ---------------------------------------------
        if c == "/":
            m = _DIRECTIVE_RE.match(state.text, state.pos)
            if m:
                name = m.group(1)
                start = state.pos
                state.pos = m.end()
                # Skip whitespace, then expect `;`
                while state.pos < state.n and state.text[state.pos] in " \t":
                    state.pos += 1
                if state.pos < state.n and state.text[state.pos] == ";":
                    state.pos += 1
                else:
                    # `/memreserve/ <addr> <size>;` — read up to `;`
                    _read_until_semicolon(state)
                full = state.text[start:state.pos]
                replacement = cb.on_directive(name, full, at_root=(state.depth == 0))
                state.out.append(replacement if replacement is not None else full)
                continue
            # Otherwise `/` could be the start of a comment (handled by
            # _skip_trivia) or division (DTS has no division).  Be safe.
            state.out.append(c)
            state.pos += 1
            continue

        # --- Reference augmentation:  &label { ... }; -------------------
        if c == "&":
            start = state.pos
            state.pos += 1
            # Read label / path
            while state.pos < state.n and state.text[state.pos] not in " \t\r\n{":
                state.pos += 1
            # Skip whitespace
            while state.pos < state.n and state.text[state.pos] in " \t\r\n":
                state.pos += 1
            if state.pos < state.n and state.text[state.pos] == "{":
                decl_text = state.text[start:state.pos]  # `&label` part
                # Let the callback rewrite if needed (rarely useful for
                # references, but consistent with node handling).
                rewritten = cb.on_node_open("&", decl_text) or decl_text
                state.out.append(rewritten)
                state.out.append("{")
                state.pos += 1
                state.depth += 1
                continue
            # No `{` — emit verbatim.  This handles `prop = &label;` etc.
            state.out.append(state.text[start:state.pos])
            continue

        # --- Labelled or bare declaration --------------------------------
        # Could be:
        #   label: name { ... };
        #   name { ... };
        #   name = value;
        #   name;
        start = state.pos
        label_match = _LABEL_RE.match(state.text, state.pos)
        label_text = ""
        if label_match:
            label_text = label_match.group(0)
            state.pos = label_match.end()
            # Mid-token trivia: don't emit here — it'll be captured in
            # the token text (state.text[start:state.pos]) and emitted
            # once when we append that text.
            _skip_trivia(state, emit=False)

        # Read the name token (node or property name).  Property names
        # may contain `#` (e.g. `#address-cells`), node names may end
        # with `@unit`.
        name_start = state.pos
        while state.pos < state.n and re.match(
            r"[" + _PROP_NAME_CHARS + r"@]", state.text[state.pos]
        ):
            state.pos += 1
        name = state.text[name_start:state.pos]

        if not name:
            # Unknown char — emit one char and continue (defensive).
            state.out.append(state.text[start:start + 1])
            state.pos = start + 1
            continue

        # Peek at what follows the name (mid-token trivia, no emit)
        _skip_trivia(state, emit=False)
        nxt = state.text[state.pos] if state.pos < state.n else ""

        if nxt == "{":
            # Node declaration
            node_decl_text = state.text[start:state.pos]  # `label: name` part
            # Decide whether this is a fixup subtree we should drop
            # (only at root level).
            if state.depth == 0 and name in FIXUP_NODE_NAMES:
                full_block_start = start
                # consume the `{ ... }` block
                _read_balanced(state, "{", "}")
                while state.pos < state.n and state.text[state.pos] in " \t\r\n":
                    state.pos += 1
                if state.pos < state.n and state.text[state.pos] == ";":
                    state.pos += 1
                full_block = state.text[full_block_start:state.pos]
                replacement = cb.on_fixup_subtree(name, full_block)
                if replacement is None:
                    replacement = ""
                state.out.append(replacement)
                continue

            # Normal node — let the callback rewrite the name if needed.
            rewritten = cb.on_node_open(name, node_decl_text) or node_decl_text
            state.out.append(rewritten)
            state.out.append("{")
            state.pos += 1  # consume `{`
            state.depth += 1
            continue

        if nxt == "=":
            # Property with value: `name = value, value, ...;`
            full_decl_start = start
            body = _read_until_semicolon(state)
            full_decl = state.text[full_decl_start:state.pos]
            replacement = cb.on_property(name, full_decl)
            state.out.append(replacement if replacement is not None else full_decl)
            continue

        if nxt == ";":
            # Property with no value: `name;`  (boolean prop)
            state.pos += 1  # consume `;`
            full_decl = state.text[start:state.pos]
            replacement = cb.on_property(name, full_decl)
            state.out.append(replacement if replacement is not None else full_decl)
            continue

        # Otherwise — unexpected token after name.  Emit the text we've
        # consumed verbatim and continue.  This is the "be permissive"
        # fallback for any construct we don't recognise.
        state.out.append(state.text[start:state.pos])
        continue

    return "".join(state.out)


# ---------------------------------------------------------------------------
# Transformers
# ---------------------------------------------------------------------------

class AndroidToADTPatcher(ScanCallbacks):
    """Apply the Android-DTS → ADT-compatible-DTS transformations."""

    def __init__(
        self,
        max_name_len: int = DEFAULT_MAX_NAME_LEN,
        strip_directives: bool = True,
        strip_fixups: bool = True,
        drop_phandle_duplicates: bool = True,
        extra_drop_props: Optional[List[str]] = None,
        verbose: bool = False,
    ):
        self.max_name_len = max_name_len
        self.strip_directives = strip_directives
        self.strip_fixups = strip_fixups
        self.drop_phandle_duplicates = drop_phandle_duplicates
        self.extra_drop_props = set(extra_drop_props or [])
        self.verbose = verbose

        self.stats = PatchStats()
        self._truncate_seen: dict = {}  # original -> truncated (per kind)
        # Track which props each *open* node has declared, so we can
        # spot `phandle` + `linux,phandle` pairs.  Stack because nodes
        # nest.
        self._node_prop_stack: List[set] = []
        # Track props we've *dropped* on the current node so the
        # de-dup logic doesn't fire twice.
        self._node_dropped_stack: List[set] = []

    # --- helpers ------------------------------------------------------

    def _truncate_name(self, name: str, kind: str) -> str:
        """Truncate a name to max_name_len, preserving the `@unit` part
        for node names and using a collision-avoidance suffix.

        Returns the (possibly truncated) name.  Updates stats.
        """
        if kind == "node":
            # Preserve the @unit part.
            if "@" in name:
                base, at, unit = name.partition("@")
                if len(base) <= self.max_name_len:
                    return name
                # Truncate base, keep unit.
                truncated_base = self._truncate_with_collisions(base, kind)
                return f"{truncated_base}@{unit}"
            else:
                if len(name) <= self.max_name_len:
                    return name
                return self._truncate_with_collisions(name, kind)
        else:  # property
            if len(name) <= self.max_name_len:
                return name
            return self._truncate_with_collisions(name, kind)

    def _truncate_with_collisions(self, name: str, kind: str) -> str:
        """Truncate to max_name_len, leaving room for a 2-char collision
        suffix (e.g. `~1`, `~2`) if needed."""
        # Reserve 3 chars for safety: `~NN` (up to 99 collisions).
        reserve = 3 if len(name) > self.max_name_len - 1 else 0
        limit = max(1, self.max_name_len - reserve)
        base = name[:limit]

        # First truncation: just `base`.
        candidate = base
        suffix = 0
        # If `candidate` is already taken by a *different* original name,
        # append a numeric suffix.
        seen_for_kind = self._truncate_seen
        while any(
            orig != name and tr == candidate
            for orig, (tr, k) in seen_for_kind.items()
            if k == kind
        ):
            suffix += 1
            s = f"~{suffix}"
            candidate = (base[: self.max_name_len - len(s)] + s)
            if suffix > 99:
                # Give up — append a hash-derived suffix instead.
                import hashlib
                h = hashlib.md5(name.encode()).hexdigest()[:4]
                candidate = base[: self.max_name_len - 5] + "~" + h
                break
        seen_for_kind[name] = (candidate, kind)
        self.stats.rename_map[name] = candidate
        if kind == "node":
            self.stats.node_names_truncated += 1
        else:
            self.stats.prop_names_truncated += 1
        if self.verbose:
            sys.stderr.write(
                f"  truncate {kind} name: {name!r} -> {candidate!r}\n"
            )
        return candidate

    # --- callbacks ----------------------------------------------------

    def on_directive(self, name, full_text, at_root):
        if self.strip_directives and name in STRIPPED_DIRECTIVES:
            self.stats.directives_stripped += 1
            if self.verbose:
                sys.stderr.write(f"  strip directive: /{name}/\n")
            return ""  # delete
        return None  # keep

    def on_fixup_subtree(self, name, full_text):
        if self.strip_fixups and name in FIXUP_NODE_NAMES:
            self.stats.nodes_deleted += 1
            if self.verbose:
                sys.stderr.write(f"  drop fixup subtree: {name}\n")
            return ""  # delete
        return None

    def on_node_open(self, name, full_text):
        # Push a fresh prop-tracking frame for this node.
        self._node_prop_stack.append(set())
        self._node_dropped_stack.append(set())

        truncated = self._truncate_name(name, "node")
        if truncated != name:
            # Replace the name within the full_text.  full_text looks
            # like `label: name` (with optional whitespace).
            return full_text.replace(name, truncated, 1)
        return None

    def on_node_exit(self):
        if self._node_prop_stack:
            self._node_prop_stack.pop()
            self._node_dropped_stack.pop()

    def on_property(self, name, full_decl):
        # Track for de-dup.
        if self._node_prop_stack:
            self._node_prop_stack[-1].add(name)

        # Drop extra-drop-props (e.g. --drop-props phandle,interrupt-map).
        if name in self.extra_drop_props:
            self.stats.props_deleted += 1
            if self.verbose:
                sys.stderr.write(f"  drop prop (extra): {name}\n")
            return ""

        # De-dup phandle / linux,phandle.  When both appear on the same
        # node, keep whichever came first and drop the second one.  The
        # old dtc fork auto-generates both names in BOTH mode anyway, so
        # emitting a manual one alongside the auto-generated ones just
        # produces redundant properties in the ADT.
        if self.drop_phandle_duplicates and self._node_prop_stack:
            seen = self._node_prop_stack[-1]
            dropped = self._node_dropped_stack[-1]
            if name == "phandle" and "linux,phandle" in seen and "phandle" not in dropped:
                self.stats.props_deleted += 1
                dropped.add("phandle")
                if self.verbose:
                    sys.stderr.write(
                        "  drop prop (dup of linux,phandle): phandle\n"
                    )
                return ""
            if name == "linux,phandle" and "phandle" in seen and "linux,phandle" not in dropped:
                self.stats.props_deleted += 1
                dropped.add("linux,phandle")
                if self.verbose:
                    sys.stderr.write(
                        "  drop prop (dup of phandle): linux,phandle\n"
                    )
                return ""

        # Truncate long prop names.
        truncated = self._truncate_name(name, "property")
        if truncated != name:
            # Replace name within full_decl.  full_decl looks like
            # `name = value;` or `name;` (with optional `label:` prefix).
            return full_decl.replace(name, truncated, 1)
        return None


# ---------------------------------------------------------------------------
# Preprocessor
# ---------------------------------------------------------------------------

def preprocess_dts(
    input_path: Path,
    output_path: Path,
    include_dirs: List[Path],
    defines: List[str],
    undefs: List[str],
    cpp_cmd: Optional[str] = None,
) -> None:
    """Run the C preprocessor on input_path, write to output_path.

    Uses `gcc -E -nostdinc -undef -x assembler-with-cpp` (the same flags
    the Linux kernel uses for DTS preprocessing).  Falls back to `cpp`
    if gcc is unavailable.
    """
    if cpp_cmd is None:
        cpp_cmd = "gcc"

    # Build the command.  We deliberately don't use `-P` (suppress line
    # markers) because some DTS consumers expect `# line` markers; but
    # for our patcher we want them gone, so we DO use `-P`.
    cmd = [cpp_cmd, "-E", "-nostdinc", "-undef", "-P",
           "-x", "assembler-with-cpp"]
    for d in include_dirs:
        cmd.extend(["-I", str(d)])
    for d in defines:
        cmd.append(f"-D{d}")
    for u in undefs:
        cmd.append(f"-U{u}")
    cmd.append(str(input_path))

    try:
        result = subprocess.run(
            cmd,
            check=True,
            capture_output=True,
            text=True,
        )
    except FileNotFoundError:
        # gcc not present — try cpp directly.
        alt_cmd = ["cpp", "-P", "-nostdinc", "-undef"]
        for d in include_dirs:
            alt_cmd.extend(["-I", str(d)])
        for d in defines:
            alt_cmd.append(f"-D{d}")
        for u in undefs:
            alt_cmd.append(f"-U{u}")
        alt_cmd.append(str(input_path))
        result = subprocess.run(alt_cmd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        sys.stderr.write(f"cpp failed (exit {e.returncode}):\n")
        sys.stderr.write(e.stderr or "")
        sys.exit(1)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(result.stdout)


# ---------------------------------------------------------------------------
# dtc-AppleDeviceTree invocation
# ---------------------------------------------------------------------------

def run_dtc(
    dtc_path: Path,
    dts_path: Path,
    output_dtb: Path,
    extra_args: Optional[List[str]] = None,
) -> None:
    """Invoke dtc to compile the patched .dts into an Apple DeviceTree
    binary.  The darwin-on-arm fork writes the ADT (not a regular FDT)
    when given `-O dtb`, so the output filename ending in `.dtb` is a
    misnomer — it's actually an ADT blob.
    """
    cmd = [str(dtc_path), "-O", "dtb", "-o", str(output_dtb), "-f"]
    if extra_args:
        cmd.extend(extra_args)
    cmd.append(str(dts_path))

    # Ensure the output directory exists (dtc won't create it).
    output_dtb.parent.mkdir(parents=True, exist_ok=True)

    sys.stderr.write(f"  invoking: {' '.join(shlex.quote(a) for a in cmd)}\n")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
    except FileNotFoundError:
        sys.stderr.write(f"  ERROR: dtc binary not found at {dtc_path}\n")
        sys.exit(1)
    if result.stderr:
        sys.stderr.write("  dtc stderr:\n")
        for line in result.stderr.splitlines():
            sys.stderr.write(f"    {line}\n")
    if result.returncode != 0:
        # Don't abort the whole batch — just record the failure and let
        # the caller continue with the next file.  This is important
        # because the dtc fork fails on some legitimate-but-modern DTS
        # constructs (e.g. `&label { ... }` reference augmentation),
        # and we don't want one bad file to prevent processing of the
        # rest of a kernel DTS tree.
        sys.stderr.write(f"  dtc failed (exit {result.returncode}); continuing with next file\n")
        return


# ---------------------------------------------------------------------------
# Directory walking
# ---------------------------------------------------------------------------

# File extensions we recognise as device-tree source.
DTS_SOURCE_EXTS = {".dts", ".dtsi"}
# Header extensions — usually #included by .dts/.dtsi files, but might
# contain DTS fragments.  We patch them too (they're plain text and the
# patcher is permissive), but we DON'T invoke dtc on them.
HEADER_EXTS = {".h", ".hpp", ".dtsi.h"}
# All extensions we'll consider when walking a directory.
ALL_DTS_LIKE_EXTS = DTS_SOURCE_EXTS | HEADER_EXTS


def walk_dts_tree(
    root: Path,
    recurse: bool = True,
    include_headers: bool = True,
    dts_only: bool = False,
) -> List[Path]:
    """Walk `root` and return a sorted list of DTS-related files.

    Args:
        root: Directory to walk.
        recurse: If True (default), descend into subdirectories.
        include_headers: If True, include `.h` files in the result.
            Set False to skip headers.
        dts_only: If True, only return `.dts` files (top-level board
            files).  Skips `.dtsi` and `.h`.  Overrides include_headers.

    Returns a sorted list of absolute Paths.
    """
    if not root.is_dir():
        return [root]

    if dts_only:
        exts = {".dts"}
    elif include_headers:
        exts = ALL_DTS_LIKE_EXTS
    else:
        exts = DTS_SOURCE_EXTS

    results: List[Path] = []
    if recurse:
        for dirpath, dirnames, filenames in os.walk(root):
            # Sort dirnames in place so os.walk descends in deterministic order.
            dirnames.sort()
            for fn in sorted(filenames):
                p = Path(dirpath) / fn
                if p.suffix.lower() in exts:
                    results.append(p)
    else:
        for fn in sorted(os.listdir(root)):
            p = root / fn
            if p.is_file() and p.suffix.lower() in exts:
                results.append(p)
    return results


def collect_inputs(
    raw_inputs: List[Path],
    recurse: bool,
    dts_only: bool,
    include_headers: bool,
) -> Tuple[List[Path], List[Path]]:
    """Expand the raw CLI inputs into (file_list, auto_include_dirs).

    - Files are kept as-is.
    - Directories are walked per the recurse/dts_only/include_headers
      flags.  Each directory (and any subdirs containing `.dtsi`/`.h`
      files) is also added to auto_include_dirs so cpp can find
      `#include "foo.dtsi"` and `#include <dt-bindings/...>`.

    For each input file we also add ALL of its ancestor directories up
    to (and including) the input root to the auto-include list.  This
    is critical because a .dts file might do `#include <dt-bindings/...>`
    where `<dt-bindings>` lives several levels up from the .dts file.

    Returns (sorted_unique_files, auto_include_dirs).
    """
    files: List[Path] = []
    auto_includes: List[Path] = []

    for inp in raw_inputs:
        if inp.is_dir():
            auto_includes.append(inp)
            # Always walk the FULL tree (regardless of dts_only /
            # include_headers) so we can collect every directory that
            # might contain a #include target.  We'll filter the file
            # list separately below.
            full_walk = walk_dts_tree(
                inp,
                recurse=recurse,
                include_headers=True,
                dts_only=False,
            )
            # Build the actual processing list per user flags.
            walked = [
                f for f in full_walk
                if (dts_only and f.suffix.lower() == ".dts")
                or (not dts_only and (
                    f.suffix.lower() in DTS_SOURCE_EXTS
                    or (include_headers and f.suffix.lower() in HEADER_EXTS)
                ))
            ]
            files.extend(walked)
            # For every file in the FULL tree (including headers and
            # .dtsi we're NOT going to process), add ALL ancestor
            # directories up to (and including) the input root to the
            # auto-include list.  This is critical: a .dts file may
            # #include <dt-bindings/...> where <dt-bindings> is several
            # levels above the .dts file, and we need cpp to find it.
            for f in full_walk:
                parent = f.parent
                while parent.is_relative_to(inp) or parent == inp:
                    if parent not in auto_includes:
                        auto_includes.append(parent)
                    if parent == inp:
                        break
                    parent = parent.parent
        elif inp.is_file():
            files.append(inp)
            # Add the file's parent directory so sibling #includes work.
            # We DON'T walk the parent dir for other files — the user
            # asked us to process one specific file, not its whole tree.
            # (Use directory mode if you want the whole tree.)
            parent = inp.parent
            auto_includes.append(parent)
            # If user-provided --include-dir entries exist, ancestors
            # of those will already cover deeper includes; we just need
            # the immediate parent here.
        else:
            sys.stderr.write(f"WARNING: input not found: {inp}\n")

    # Dedupe while preserving order.
    seen = set()
    unique_files: List[Path] = []
    for f in files:
        rp = f.resolve()
        if rp not in seen:
            seen.add(rp)
            unique_files.append(f)

    seen_dirs = set()
    unique_includes: List[Path] = []
    for d in auto_includes:
        rd = d.resolve()
        if rd not in seen_dirs:
            seen_dirs.add(rd)
            unique_includes.append(d)

    return sorted(unique_files), unique_includes


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def build_argparser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="dts2adt.py",
        description=(
            "Patch Android/Linux kernel device-tree source (.dts/.dtsi) so the "
            "darwin-on-arm dtc-AppleDeviceTree fork can compile it into an "
            "Apple DeviceTree binary.\n\n"
            "Inputs may be files OR directories.  When a directory is given, "
            "it is walked recursively for .dts/.dtsi/.h files (see --recurse, "
            "--dts-only, --no-headers)."
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=(
            "Examples:\n"
            "  # Single file:\n"
            "  python3 dts2adt.py -o patched.dts input.dts\n\n"
            "  # Whole directory tree (auto-finds .dts/.dtsi/.h):\n"
            "  python3 dts2adt.py --mirror -O out/ kernel/arch/arm64/boot/dts/\n\n"
            "  # End-to-end with cpp + dtc:\n"
            "  python3 dts2adt.py \\\n"
            "      --include-dir /kern/include \\\n"
            "      --dtc /path/to/dtc-AppleDeviceTree/dtc \\\n"
            "      --mirror -O out/ --dtb-dir adt/ \\\n"
            "      kernel/arch/arm64/boot/dts/\n"
        ),
    )
    p.add_argument("inputs", nargs="+", type=Path,
                   help="One or more .dts/.dtsi files OR directories containing "
                        "them.  Directories are walked per --recurse/--dts-only/"
                        "--no-headers.")
    p.add_argument("-I", "--include-dir", action="append", type=Path, default=[],
                   dest="include_dirs", metavar="DIR",
                   help="Add DIR to cpp's include search path (repeatable). "
                        "Note: when you pass a directory as input, all of its "
                        "subdirectories containing .dtsi/.h files are added "
                        "automatically.")
    p.add_argument("-D", "--define", action="append", default=[],
                   dest="defines", metavar="SYM[=VAL]",
                   help="Predefine a cpp symbol (repeatable)")
    p.add_argument("-U", "--undef", action="append", default=[],
                   dest="undefs", metavar="SYM",
                   help="Undefine a cpp symbol (repeatable)")
    p.add_argument("--cpp", default="gcc", metavar="PATH",
                   help="C preprocessor to invoke (default: gcc)")
    p.add_argument("--no-cpp", action="store_true",
                   help="Skip cpp; input is already preprocessed")

    # --- Output target flags -----------------------------------------
    g_out = p.add_argument_group("Output targets")
    g_out.add_argument("-o", "--output", type=Path, metavar="FILE",
                       help="Output patched .dts (single input file only).")
    g_out.add_argument("-O", "--output-dir", type=Path, metavar="DIR",
                       help="Write patched files into DIR (flat; multiple inputs).")
    g_out.add_argument("--mirror", action="store_true",
                       help="Mirror the input directory structure under --output-dir. "
                            "Requires --output-dir and at least one directory input.")
    g_out.add_argument("--dtb", metavar="FILE", type=Path,
                       help="Output .dtb path (single input only).")
    g_out.add_argument("--dtb-dir", metavar="DIR", type=Path,
                       help="Directory to write .dtb files into (multiple inputs). "
                            "Default: same dir as each patched .dts.")

    # --- Directory walking flags -------------------------------------
    g_walk = p.add_argument_group("Directory walking")
    g_walk.add_argument("--recurse/--no-recurse", dest="recurse", default=True,
                        help="Recurse into subdirectories (default: on).")
    g_walk.add_argument("--dts-only", action="store_true",
                        help="Only process .dts files (skip .dtsi and .h). "
                             "Useful when you only want top-level board files.")
    g_walk.add_argument("--no-headers", action="store_true",
                        help="Skip .h files when walking directories.  .dts and "
                             ".dtsi are still processed.")
    g_walk.add_argument("--auto-include/--no-auto-include", dest="auto_include",
                        default=True,
                        help="Automatically add directories containing .dtsi/.h "
                             "files to cpp's -I path (default: on).")

    # --- dtc integration ---------------------------------------------
    g_dtc = p.add_argument_group("dtc invocation")
    g_dtc.add_argument("--dtc", type=Path, metavar="PATH",
                       help="Path to the dtc-AppleDeviceTree binary; if set, also "
                            "compile each top-level patched .dts to a .dtb (ADT blob). "
                            "Only .dts files are compiled — .dtsi and .h are skipped.")
    g_dtc.add_argument("--dtc-arg", action="append", default=[], dest="dtc_args",
                       metavar="ARG",
                       help="Extra arg passed through to dtc (repeatable)")

    # --- Patch tuning ------------------------------------------------
    g_patch = p.add_argument_group("Patch tuning")
    g_patch.add_argument("--max-name-len", type=int, default=DEFAULT_MAX_NAME_LEN,
                         help="Maximum node/property name length (default: 31)")
    g_patch.add_argument("--keep-directives", action="store_true",
                         help="Don't strip /plugin/, /delete-node/, /delete-property/")
    g_patch.add_argument("--keep-fixups", action="store_true",
                         help="Don't drop /__local_fixups__/, /__symbols__/, /__fixups__/")
    g_patch.add_argument("--drop-phandle", action="store_true",
                         help="Drop both `phandle` and `linux,phandle` props on every "
                              "node (the old dtc auto-generates them in BOTH mode)")
    g_patch.add_argument("--drop-props", default="", metavar="LIST",
                         help="Comma-separated list of property names to always drop")

    # --- Misc --------------------------------------------------------
    g_misc = p.add_argument_group("Misc")
    g_misc.add_argument("--dry-run", action="store_true",
                        help="Don't write any files; just print stats")
    g_misc.add_argument("-v", "--verbose", action="store_true",
                        help="Print every change as it happens")
    g_misc.add_argument("-q", "--quiet", action="store_true",
                        help="Suppress the per-file summary")
    return p


def process_one(
    input_path: Path,
    output_path: Optional[Path],
    args: argparse.Namespace,
    extra_include_dirs: Optional[List[Path]] = None,
    force_dtb_path: Optional[Path] = None,
    skip_dtc: bool = False,
) -> Tuple[Path, PatchStats]:
    """Process a single input file.

    Args:
        input_path: The .dts/.dtsi/.h file to process.
        output_path: Where to write the patched file, or None to pick a
            default (input + ".patched.dts").
        args: Parsed CLI args.
        extra_include_dirs: Extra cpp -I paths discovered by directory
            walking.  Merged with args.include_dirs.
        force_dtb_path: Override the .dtb output path (used for single
            --dtb mode).
        skip_dtc: If True, don't invoke dtc even if --dtc is set.  Used
            for .dtsi/.h files, which aren't standalone-compilable.

    Returns (patched_dts_path, stats).
    """
    if args.verbose:
        sys.stderr.write(f"--- processing {input_path}\n")

    # 1. Preprocess if needed.
    if args.no_cpp:
        pre_path = input_path
        cpp_invoked = False
    else:
        # Write preprocessed output to a sibling .preprocessed.dts file.
        pre_path = output_path.with_suffix(".preprocessed.dts") if output_path else \
                   input_path.with_suffix(".preprocessed.dts")
        if args.dry_run:
            pre_path = Path("/tmp") / (input_path.stem + ".preprocessed.dts")
        all_includes = list(args.include_dirs) + list(extra_include_dirs or [])
        preprocess_dts(
            input_path,
            pre_path,
            all_includes,
            args.defines,
            args.undefs,
            cpp_cmd=args.cpp,
        )
        cpp_invoked = True

    src = pre_path.read_text()

    # 2. Patch.
    extra_drop = [p.strip() for p in args.drop_props.split(",") if p.strip()]
    if args.drop_phandle:
        extra_drop.extend(["phandle", "linux,phandle"])

    patcher = AndroidToADTPatcher(
        max_name_len=args.max_name_len,
        strip_directives=not args.keep_directives,
        strip_fixups=not args.keep_fixups,
        drop_phandle_duplicates=True,
        extra_drop_props=extra_drop,
        verbose=args.verbose,
    )
    patcher.stats.cpp_invoked = cpp_invoked
    patched = scan_dts(src, patcher)

    # 3. Write patched output.
    if output_path is None:
        # Default: sibling of input.  For .dts files, use ".patched.dts"
        # (so dtc picks it up naturally).  For .dtsi/.h, preserve the
        # original extension and insert ".patched" before it.
        if input_path.suffix.lower() == ".dts":
            output_path = input_path.with_suffix(".patched.dts")
        else:
            output_path = input_path.with_suffix(".patched" + input_path.suffix)
    if not args.dry_run:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(patched)

    # 4. Invoke dtc if requested (only for .dts files; .dtsi/.h are
    #    not standalone-compilable).
    if args.dtc is not None and not skip_dtc:
        if force_dtb_path is not None:
            dtb_path = force_dtb_path
        else:
            dtb_path = output_path.with_suffix(".dtb")
        if not args.dry_run:
            run_dtc(args.dtc, output_path, dtb_path, args.dtc_args)

    return output_path, patcher.stats


def _mirror_output_path(
    input_path: Path,
    input_root: Path,
    output_dir: Path,
    new_suffix: Optional[str] = None,
) -> Path:
    """Compute the mirrored output path for an input under input_root.

    Examples:
        input_root = /kern/dts/
        input_path = /kern/dts/qcom/sdm845-mtp.dts
        output_dir = out/
        new_suffix = .patched.dts
        => out/qcom/sdm845-mtp.patched.dts

        # new_suffix=None preserves the original extension and just
        # appends ".patched" before it:
        input_path = /kern/dts/include/foo.h
        new_suffix = None
        => out/kern/dts/include/foo.patched.h
    """
    rel = input_path.relative_to(input_root)
    out = output_dir / rel
    if new_suffix is not None:
        return out.with_suffix(new_suffix)
    # Preserve original extension, insert ".patched" before it.
    return out.with_suffix(".patched" + input_path.suffix)


def main(argv: Optional[List[str]] = None) -> int:
    args = build_argparser().parse_args(argv)

    # --- Validate flag combinations -----------------------------------
    if args.output and (len(args.inputs) > 1 or args.output_dir or args.mirror):
        sys.stderr.write("ERROR: --output cannot be combined with multiple inputs, "
                         "--output-dir, or --mirror\n")
        return 2
    if args.mirror and not args.output_dir:
        sys.stderr.write("ERROR: --mirror requires --output-dir\n")
        return 2
    if args.dtb and (len(args.inputs) > 1 or args.output_dir or args.dtb_dir or args.mirror):
        sys.stderr.write("ERROR: --dtb is single-input only; use --dtb-dir for "
                         "multiple outputs\n")
        return 2
    if args.dtb_dir and not args.dtc:
        sys.stderr.write("WARNING: --dtb-dir without --dtc has no effect (no dtc "
                         "binary to invoke)\n")

    # --- Expand directory inputs into file lists ----------------------
    all_files, auto_includes = collect_inputs(
        args.inputs,
        recurse=args.recurse,
        dts_only=args.dts_only,
        include_headers=not args.no_headers,
    )

    if not all_files:
        sys.stderr.write("ERROR: no .dts/.dtsi/.h files found in the given inputs\n")
        return 1

    if not args.quiet:
        sys.stderr.write(f"found {len(all_files)} file(s) to process")
        if auto_includes and args.auto_include:
            sys.stderr.write(f", {len(auto_includes)} auto-include dir(s)")
        sys.stderr.write("\n")
        if args.verbose:
            for f in all_files:
                sys.stderr.write(f"  - {f}\n")
            for d in auto_includes:
                sys.stderr.write(f"  +I {d}\n")

    extra_includes = auto_includes if args.auto_include else []

    # --- Identify the "input root" for mirroring ----------------------
    # If any input is a directory, use the first directory input as the
    # mirror root.  Files passed directly get mirrored as if their
    # parent directory were the root.
    input_root: Optional[Path] = None
    for inp in args.inputs:
        if inp.is_dir():
            input_root = inp
            break
    if input_root is None and args.mirror and all_files:
        # --mirror with only file inputs: use the parent of the first
        # file as the "root" so the relative path is just the filename.
        input_root = all_files[0].parent

    # --- Process each file --------------------------------------------
    total = PatchStats()
    n_files = len(all_files)
    for idx, inp in enumerate(all_files, 1):
        # Determine the output path for the patched file.
        if args.output:
            # Single-file mode.
            out = args.output
        elif args.mirror and input_root is not None:
            # Mirror mode: preserve original extension for .h/.dtsi,
            # use .patched.dts for .dts (since that's what dtc expects).
            if inp.suffix.lower() == ".dts":
                out = _mirror_output_path(inp, input_root, args.output_dir, ".patched.dts")
            else:
                out = _mirror_output_path(inp, input_root, args.output_dir, None)
        elif args.output_dir:
            # Flat output dir.
            if inp.suffix.lower() == ".dts":
                out = args.output_dir / (inp.stem + ".patched.dts")
            else:
                out = args.output_dir / (inp.stem + ".patched" + inp.suffix)
        else:
            out = None  # let process_one pick a default (sibling of input)

        # Determine whether to skip dtc for this file.
        # Only .dts files are standalone-compilable.
        skip_dtc = inp.suffix.lower() != ".dts"

        # Determine the .dtb path if applicable.
        force_dtb = None
        if args.dtb and n_files == 1 and not skip_dtc:
            force_dtb = args.dtb
        elif args.dtb_dir and not skip_dtc and args.dtc:
            if args.mirror and input_root is not None:
                force_dtb = _mirror_output_path(inp, input_root, args.dtb_dir, ".dtb")
            else:
                force_dtb = args.dtb_dir / (inp.stem + ".dtb")

        if skip_dtc and args.dtc is not None and args.verbose:
            sys.stderr.write(f"  (skipping dtc for {inp.suffix} file: {inp.name})\n")

        patched_path, stats = process_one(
            inp, out, args,
            extra_include_dirs=extra_includes,
            force_dtb_path=force_dtb,
            skip_dtc=skip_dtc,
        )

        # Aggregate totals.
        total.cpp_invoked = total.cpp_invoked or stats.cpp_invoked
        total.directives_stripped += stats.directives_stripped
        total.nodes_deleted += stats.nodes_deleted
        total.props_deleted += stats.props_deleted
        total.node_names_truncated += stats.node_names_truncated
        total.prop_names_truncated += stats.prop_names_truncated
        total.rename_map.update(stats.rename_map)

        if not args.quiet:
            sys.stderr.write(f"[{idx}/{n_files}] patched: {inp} -> {patched_path}\n")
            sys.stderr.write(stats.summary() + "\n")

    if not args.quiet and n_files > 1:
        sys.stderr.write("=== TOTAL ===\n")
        sys.stderr.write(total.summary() + "\n")

    return 0


if __name__ == "__main__":
    sys.exit(main())
