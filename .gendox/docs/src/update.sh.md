```markdown
# docgen-upgrade.sh

## Purpose
This script automates the process of upgrading the `docgen` tool to its latest version. It detects the operating system, downloads the appropriate binary from the GitHub releases page, and replaces the existing `docgen` installation with the new version.

## Usage
To use this script, ensure it has executable permissions and run it in a terminal:

```bash
chmod +x docgen-upgrade.sh
./docgen-upgrade.sh
```

## Behavior
1. **OS Detection**:  
   The script identifies the operating system using `uname -s`. It supports **Linux** and **macOS** (detected as "Darwin"), assigning the corresponding binary asset name (`docgen-linux` or `docgen-macos`). Unsupported OSes result in an error and script termination.

2. **Download**:  
   The latest version of `docgen` is downloaded from the GitHub releases page using `curl`. The binary is saved to `/tmp/docgen_new` and made executable.  
   The download URL uses the pattern:  
   `https://github.com/alonsovm44/docgen/releases/latest/download/<asset>`

3. **Locate and Replace**:  
   The script locates the existing `docgen` binary in the system's `PATH` using `which`. If not found, it exits with an error. The new binary is then moved to replace the old one. If permissions are insufficient, it attempts the operation with `sudo`.

4. **Completion**:  
   Upon successful replacement, the script confirms the upgrade is complete.

## Error Handling
- Exits with an error if the OS is unsupported.
- Exits with an error if `docgen` is not found in the `PATH`.
- Handles permission issues by attempting a `sudo` move if necessary.

## Requirements
- `curl` must be installed.
- `docgen` must be installed and accessible in the `PATH`.
- Internet connectivity to access the GitHub releases URL.
```