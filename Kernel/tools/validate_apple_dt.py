#!/usr/bin/env python3
"""
validate_apple_dt.py - Validate an Apple-format flattened device tree binary.

Parses the binary using the same logic as pexpert/gen/device_tree.c and
verifies structural integrity.
"""

import struct
import sys


ALIGN = 4


def align4(n):
    return (n + ALIGN - 1) & ~(ALIGN - 1)


def parse_tree(data, offset=0, depth=0, results=None):
    """Parse one node from the Apple DT binary format.

    Returns (next_offset, node_dict) or raises on error.
    """
    if results is None:
        results = []

    if offset + 8 > len(data):
        raise ValueError(f"Truncated: need 8 bytes at offset {offset}, have {len(data) - offset}")

    n_props, n_children = struct.unpack_from("<II", data, offset)
    pos = offset + 8

    if n_props == 0 and n_children == 0:
        return pos, None  # sentinel

    prefix = "  " * depth
    node = {"n_properties": n_props, "n_children": n_children, "props": {}, "children": []}

    # Parse properties
    for i in range(n_props):
        if pos + 36 > len(data):
            raise ValueError(f"Truncated property header at offset {pos}")

        name_bytes = data[pos:pos + 32]
        name_end = name_bytes.index(b"\x00") if b"\x00" in name_bytes else 32
        name = name_bytes[:name_end].decode("ascii")

        val_len = struct.unpack_from("<I", data, pos + 32)[0]
        pos += 36

        if pos + val_len > len(data):
            raise ValueError(f"Truncated property value for '{name}': need {val_len} bytes")

        val = data[pos:pos + val_len]
        pos += align4(val_len)

        node["props"][name] = val
        print(f"{prefix}[prop] {name} = {val_len} bytes: {val.hex()}")

    # Parse children
    for i in range(n_children):
        pos, child = parse_tree(data, pos, depth + 1, results)
        if child is None:
            raise ValueError(f"Unexpected sentinel at child {i} of node with {n_children} children")
        node["children"].append(child)

    return pos, node


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <device-tree.dtb>")
        sys.exit(1)

    path = sys.argv[1]
    with open(path, "rb") as f:
        data = f.read()

    print(f"File: {path} ({len(data)} bytes)")
    print()

    try:
        pos, root = parse_tree(data)
        print()

        # Check sentinel
        if pos == len(data):
            print("OK: Sentinel node found at end of data")
        elif pos < len(data):
            # There might be trailing data; check if it's a sentinel
            n_props, n_children = struct.unpack_from("<II", data, pos)
            if n_props == 0 and n_children == 0:
                print("OK: Sentinel node found (with trailing padding)")
            else:
                print(f"WARNING: {len(data) - pos} bytes after root node, not a sentinel")
        else:
            print(f"ERROR: Parsed {pos} bytes but file is only {len(data)} bytes")
            sys.exit(1)

        # Validate required properties
        if root:
            print()
            if "name" in root["props"]:
                name = root["props"]["name"].decode("ascii", errors="replace").rstrip("\x00")
                print(f"Root node name: {name}")
            else:
                print("WARNING: Root node has no 'name' property")

            if "compatible" in root["props"]:
                compat = root["props"]["compatible"].decode("ascii", errors="replace").rstrip("\x00")
                print(f"Root compatible: {compat}")

            if "model" in root["props"]:
                model = root["props"]["model"].decode("ascii", errors="replace").rstrip("\x00")
                print(f"Root model: {model}")

            # Check for /chosen node
            chosen_found = False
            for child in root["children"]:
                child_name = child["props"].get("name", b"").decode("ascii", errors="replace").rstrip("\x00")
                if child_name == "chosen":
                    chosen_found = True
                    print(f"/chosen node: FOUND")
                    if "firmware-version" in child["props"]:
                        fv = child["props"]["firmware-version"].decode("ascii", errors="replace").rstrip("\x00")
                        print(f"  firmware-version: {fv}")
                    if "debug-enabled" in child["props"]:
                        print(f"  debug-enabled: present")
                    break

            if not chosen_found:
                print("WARNING: /chosen node not found")

            # Check for /memory node
            memory_found = False
            for child in root["children"]:
                child_name = child["props"].get("name", b"").decode("ascii", errors="replace").rstrip("\x00")
                if child_name == "memory":
                    memory_found = True
                    print(f"/memory node: FOUND")
                    break

            if not memory_found:
                print("WARNING: /memory node not found")

            # Check for /cpus node
            cpus_found = False
            for child in root["children"]:
                child_name = child["props"].get("name", b"").decode("ascii", errors="replace").rstrip("\x00")
                if child_name == "cpus":
                    cpus_found = True
                    cpu_count = len(child["children"])
                    print(f"/cpus node: FOUND ({cpu_count} CPUs)")
                    break

            if not cpus_found:
                print("WARNING: /cpus node not found")

        print()
        print("Validation PASSED")

    except Exception as e:
        print(f"\nValidation FAILED: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
