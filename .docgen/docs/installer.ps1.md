# docgen Setup Script Documentation

## Purpose
This PowerShell script automates the installation of the `docgen` tool, preferring prebuilt release assets from GitHub Releases. If no prebuilt asset is available for the platform, it falls back to downloading the source code and provides build instructions.

## Usage
Run the script in a PowerShell environment. It handles the following tasks:

1. **Release Asset Download**: Queries GitHub Releases for a prebuilt asset matching the OS and architecture.
2. **Installation**: Downloads and installs the prebuilt asset or provides instructions for building from source if no prebuilt asset is available.

## Behavior

### Release Asset Download
- **GitHub API Query**: Queries the GitHub API for the latest release of the `docgen` repository.
- **Asset Selection**: Prefers assets matching the OS (`windows`) and architecture (`x86_64` or `x86`). If no exact match is found, it falls back to the first Windows asset.

### Installation
- **Prebuilt Asset**: If a prebuilt asset is found, it is downloaded, extracted (if necessary), and the executable is copied to the installation directory (`$InstallDir`).
- **Source Code Fallback**: If no prebuilt asset is available, the script downloads the source code archive and provides instructions for building from source.

### Installation Directory
- The installation directory defaults to `%USERPROFILE%\bin` but can be customized via the `$InstallDir` parameter.
- Ensures the installation directory exists and informs the user if it is not on the current PATH.

### Error Handling
- Stops execution on critical errors (e.g., failed API query or download) and exits with a non-zero status code.

## Example Workflow
1. Run the script.
2. If a prebuilt asset is available, it is downloaded and installed.
3. If no prebuilt asset is available, the source code is downloaded, and build instructions are provided.

## Requirements
- PowerShell 5.1 or later.
- Internet connection (for downloading assets and source code).

## Notes
- The script cleans up temporary directories after completion.
- Ensure an active internet connection for downloading assets and source code.
- If the installation directory is not on the PATH, add it manually or re-open the terminal after installation.

## Parameters
- **Owner**: GitHub repository owner (default: "alonsovm44").
- **Repo**: GitHub repository name (default: "docgen").
- **InstallDir**: Installation directory (default: `%USERPROFILE%\bin`).

## Changes from Previous Version
- Removed prerequisite checks for `curl`, `ollama`, and C++ compilers.
- Removed automatic compilation from source.
- Added preference for prebuilt release assets.
- Simplified installation process focusing on prebuilt assets and fallback to source code with manual build instructions.