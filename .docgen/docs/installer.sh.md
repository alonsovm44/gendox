# docgen Build Script Documentation

## Purpose
This script automates the installation of the `docgen` tool by preferring prebuilt release assets from GitHub Releases. If no prebuilt asset is available for the detected OS/architecture, it falls back to downloading the source code archive and provides build instructions.

## Behavior

### OS/Architecture Detection
- Detects the operating system and architecture using `uname`.
- Maps detected values to supported targets (e.g., `linux-x86_64`, `macos-arm64`).

### GitHub Release Query
- Queries the GitHub API for the latest release of the `docgen` repository.
- Uses `curl`, `wget`, or `python3` to fetch release metadata.

### Asset Selection
- Searches for a prebuilt asset matching the detected OS/architecture.
- Prefers exact matches (e.g., `docgen-linux-x86_64.tar.gz`) and falls back to shorter matches or any asset containing the OS name.

### Download and Verification
1. **Download**:  
   - Downloads the selected asset using `curl`, `wget`, or `python3`.  
2. **Checksum Verification**:  
   - If a checksum asset is available, downloads and verifies the integrity of the downloaded file using `sha256sum` or `shasum`.  

### Extraction and Installation
- Extracts the downloaded archive (supports `.tar.gz` and `.zip`).  
- Locates the `docgen` binary within the extracted files.  
- Installs the binary to `/usr/local/bin` or a user-specified directory (`LOCAL_INSTALL`).  
- Ensures the binary is executable.  

### Fallback to Source Code
- If no prebuilt asset is found, downloads the source code archive from the main branch.  
- Provides detailed instructions for building from source, including extraction, CMake, and compilation steps.  

## Usage
1. **Run the Script**:  
   Execute the script in a terminal:  
   ```bash
   ./install.sh
   ```

2. **Custom Installation Directory**:  
   Override the default installation directory using:  
   ```bash
   LOCAL_INSTALL=~/.local/bin ./install.sh
   ```

3. **Post-Installation**:  
   - After successful installation, run the tool using:  
     ```bash
     docgen --help
     ```

## Error Handling
- Exits with a non-zero status if:  
  - No downloader (`curl`, `wget`, or `python3`) is available.  
  - No prebuilt asset or source code can be downloaded.  
  - The binary cannot be located in the extracted archive.  

## Dependencies
- **Downloader**: `curl`, `wget`, or `python3` (required for downloading assets).  
- **Extraction Tools**: `tar` (for `.tar.gz`), `unzip` (for `.zip`).  
- **Checksum Verification**: `sha256sum` or `shasum` (optional but recommended).  

## Notes
- The script does not require `git` for installation.  
- If no prebuilt asset is available for your platform, consider opening an issue in the repository to request support.  
- Building from source requires `cmake`, `make`, and a C++ toolchain. The script does not automate the build process by default but provides clear instructions.