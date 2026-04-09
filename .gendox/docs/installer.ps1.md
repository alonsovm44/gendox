# docgen Setup Script Documentation

## Purpose
This PowerShell script automates the installation of the `docgen` tool, preferring prebuilt release assets from GitHub Releases. If no prebuilt asset is available for the platform, it falls back to downloading the source code and provides build instructions. Additionally, the script offers to install Ollama if it is not already installed, as it is highly recommended for local AI processing.

## Usage
Run the script in a PowerShell environment. It handles the following tasks:

1. **Ollama Installation Prompt**: Checks if Ollama is installed and offers to install it if not found.
2. **Release Asset Download**: Queries GitHub Releases for a prebuilt asset matching the OS and architecture.
3. **Installation**: Downloads and installs the prebuilt asset or builds from source if no prebuilt asset is available.

## Behavior

### Ollama Installation Prompt
- **Check for Ollama**: Verifies if Ollama is installed by checking for the `ollama` command.
- **Installation Offer**: If Ollama is not found, prompts the user to install it. If the user agrees, downloads and runs the Ollama installer.

### Release Asset Download
- **GitHub API Query**: Queries the GitHub API for the latest release of the `docgen` repository.
- **Asset Selection**: Prefers assets matching the OS (`windows`) and architecture (`amd64`, `arm64`, or `x86`). If no exact match is found, it falls back to source code installation.

### Installation
- **Prebuilt Asset**: If a prebuilt asset is found, it is downloaded and placed directly into the installation directory (`$InstallDir`).
- **Source Code Fallback**: If no prebuilt asset is available, the script attempts to build `docgen` from source. It checks for required build tools (`gcc`, `g++`, `clang`, `clang++`, and `make`), downloads the source code and necessary dependencies, compiles the project, and installs the executable.

### Installation Directory
- The installation directory defaults to `%USERPROFILE%\.local\bin` but can be customized via the `$InstallDir` variable.
- Ensures the installation directory exists and informs the user if it is not on the current PATH.

### Error Handling
- Stops execution on critical errors (e.g., failed API query, download, or build) and exits with a non-zero status code.

## Example Workflow
1. Run the script.
2. If Ollama is not installed, the script offers to install it.
3. If a prebuilt asset is available, it is downloaded and installed.
4. If no prebuilt asset is available, the script attempts to build `docgen` from source.

## Requirements
- PowerShell 5.1 or later.
- Internet connection (for downloading assets, source code, and Ollama installer).
- C/C++ compilers (`gcc`, `g++`, `clang`, or `clang++`) and `make` utility for building from source (if fallback is required).

## Notes
- The script cleans up temporary directories after completion.
- Ensure an active internet connection for downloading assets, source code, and Ollama installer.
- If the installation directory is not on the PATH, add it manually or re-open the terminal after installation.

## Parameters
- **Owner**: GitHub repository owner (default: "alonsovm44").
- **Repo**: GitHub repository name (default: "docgen").
- **InstallDir**: Installation directory (default: `%USERPROFILE%\.local\bin`).

## Changes from Previous Version
- Added prompt to install Ollama if not found.
- Updated installation directory default to `%USERPROFILE%\.local\bin`.
- Enhanced source build process to include both C and C++ compilers, and added support for tree-sitter language parsers.
- Simplified prebuilt asset installation process.
- Improved error handling and user feedback during the installation process.

**Updated Code Changes:**

- The script now checks for both C (`gcc` or `clang`) and C++ (`g++` or `clang++`) compilers during the source build process.
- Added support for compiling tree-sitter language parsers (C, C++, Python, JavaScript, TypeScript, Go, Rust) along with the main `docgen` source code.
- Improved compiler flag handling, including separate flags for C and C++ compilation.
- The build process now explicitly compiles both C and C++ source files, ensuring all dependencies are built correctly.
- Enhanced error checking during compilation and linking to provide more detailed feedback.