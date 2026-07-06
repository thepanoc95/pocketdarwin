import re
from pathlib import Path

import requests

# ============================================
# Configuration
# ============================================

OWNER = "LineageOS"
REPO = "android_kernel_motorola_msm8916"

# Starting DTSI file
START_FILE = "arch/arm/boot/dts/qcom/msm8916-harpia.dtsi"

# Where files will be saved
OUTPUT_DIR = Path("kernel_dts")

# ============================================

include_regex = re.compile(r'#include\s+[<"]([^">]+)[">]')

visited = set()
repo_files = {}

# -----------------------------------------------------------

print("Getting repository information...")

repo_info = requests.get(
    f"https://api.github.com/repos/{OWNER}/{REPO}"
)
repo_info.raise_for_status()

BRANCH = repo_info.json()["default_branch"]

print(f"Using default branch: {BRANCH}")

RAW_BASE = (
    f"https://raw.githubusercontent.com/"
    f"{OWNER}/{REPO}/{BRANCH}"
)

TREE_API = (
    f"https://api.github.com/repos/"
    f"{OWNER}/{REPO}/git/trees/{BRANCH}?recursive=1"
)

# -----------------------------------------------------------


def fetch_repo_tree():
    print("Fetching repository tree...")

    r = requests.get(TREE_API)
    r.raise_for_status()

    for item in r.json()["tree"]:
        if item["type"] == "blob":
            repo_files[item["path"]] = item["path"]


def find_include(current_file: str, include_name: str):
    current_dir = Path(current_file).parent

    # Relative include
    candidate = str(current_dir / include_name)

    if candidate in repo_files:
        return candidate

    # Absolute repository path
    if include_name in repo_files:
        return include_name

    # Search by filename
    filename = Path(include_name).name

    matches = [
        path for path in repo_files
        if Path(path).name == filename
    ]

    if len(matches) == 1:
        return matches[0]

    if len(matches) > 1:
        print(f"Warning: multiple matches for {filename}, using {matches[0]}")
        return matches[0]

    return None


def download(path: str):
    if path in visited:
        return

    visited.add(path)

    print(f"Downloading {path}")

    url = f"{RAW_BASE}/{path}"

    r = requests.get(url)

    if r.status_code != 200:
        print(f"Failed ({r.status_code}): {path}")
        return

    text = r.text

    outfile = OUTPUT_DIR / path
    outfile.parent.mkdir(parents=True, exist_ok=True)
    outfile.write_text(text, encoding="utf-8")

    includes = include_regex.findall(text)

    for inc in includes:
        # Skip kernel header includes
        if inc.startswith("dt-bindings/"):
            continue

        resolved = find_include(path, inc)

        if resolved is None:
            print(f"Couldn't resolve include: {inc}")
            continue

        download(resolved)


def main():
    OUTPUT_DIR.mkdir(exist_ok=True)

    fetch_repo_tree()

    if START_FILE not in repo_files:
        print(f"\nERROR: {START_FILE} was not found on branch '{BRANCH}'.")
        print("Available files ending with 'harpia.dtsi':")

        found = False
        for path in repo_files:
            if path.endswith("harpia.dtsi"):
                print(" ", path)
                found = True

        if not found:
            print("  (none found)")

        return

    download(START_FILE)

    print("\n===================================")
    print(f"Downloaded {len(visited)} files.")
    print(f"Saved to: {OUTPUT_DIR.resolve()}")


if __name__ == "__main__":
    main()