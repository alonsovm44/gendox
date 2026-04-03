# docgen Build Script Documentation

## Purpose
This script automates the installation of the `docgen` tool by preferring prebuilt release assets from GitHub Releases. If no prebuilt asset is available for the detected OS/architecture, it falls back to downloading and building the source code.  
The script also ensures core dependencies are installed automatically and optionally installs **Ollama** for local AI processing.

## Behavior

### Dependency Installation
- Automatically checks for required tools: `curl`, `tar`, and `grep`.
- If missing, attempts installation using the system’s available package manager (`apt-get`, `dnf`, `yum`, `pacman`, `zypper`, or `brew`).
- If no supported package manager is detected, the script exits with an error and instructs the user to install the dependency manually.

### Optional Ollama Installation
- Detects whether `ollama` is installed.
- If missing, prompts the user to install it.
- If accepted, installs Ollama via its official installation script.
- If declined, installation continues without Ollama.

### OS/Architecture Detection
- Detects the operating system and architecture using `uname`.
- Normalizes architecture names to `amd64` or `arm64` for compatibility with release assets.

### GitHub Release Query
- Queries the GitHub API for the latest release of the `docgen` repository.
- Uses `curl` to fetch release metadata.
- Searches for a prebuilt asset matching the detected OS and architecture.

### Asset Selection
- Selects the first matching asset containing both the OS and architecture in its filename.
- If no matching asset is found, the script falls back to building from source.

### Download and Installation
1. **Download**:  
   - Downloads the selected prebuilt binary directly into the installation directory (`$HOME/.local/bin`).  

2. **Installation**:  
   - Ensures the binary is executable.  
   - If download fails, automatically falls back to building from source.

### Fallback to Source Code
If no prebuilt binary is available or the download fails, the script:

1. **Checks Build Dependencies**  
   - Ensures a C++ compiler (`g++` or `clang++`) is available.  
   - On macOS, instructs the user to install Xcode Command Line Tools if no compiler is found.  
   - Ensures `make` is installed.  
   - Installs missing tools when possible.

2. **Local Source Build**  
   - If `src/main.cpp` and `Makefile` exist in the current directory, builds directly using `make`.  
   - Installs the resulting binary to `$HOME/.local/bin`.

3. **Remote Source Build**  
   - Downloads the latest source archive from GitHub.  
   - Downloads required **tree-sitter** language grammars (C, C++, Python, JavaScript, TypeScript, Go, Rust).  
   - Builds using `make`.  
   - Installs the resulting binary to `$HOME/.local/bin`.  
   - Cleans up temporary build directories.

### PATH Warning
- After installation, the script checks whether `$HOME/.local/bin` is in the user’s PATH.
- If not, it prints instructions for adding it to the shell configuration.

## Usage
1. **Run the Script**:  
   Execute the script in a terminal:  
   ```bash
   ./install.sh
   ```

2. **Installation Directory**:  
   The script installs to:  
   ```
   ~/.local/bin
   ```
   This directory is created automatically if missing.

3. **Post-Installation**:  
   Run the tool using:  
   ```bash
   docgen --help
   ```

## Error Handling
- Exits with a non-zero status if:  
  - No supported package manager is found when installing dependencies.  
  - Required build tools cannot be installed.  
  - The prebuilt binary cannot be downloaded.  
  - Source code cannot be downloaded.  
  - The build process fails.  

## Dependencies
- **Downloader**: `curl` (required).  
- **Extraction Tools**: `tar`.  
- **Build Tools** (for source fallback):  
  - `g++` or `clang++`  
  - `make`  
- **tree-sitter grammars**: Automatically downloaded during source build.  

## Notes
- The script uses a Makefile-based build system.  
- The script attempts to install missing dependencies automatically when possible.  
- Ollama installation is optional but recommended for enhanced local AI functionality.  
- If `$HOME/.local/bin` is not in your PATH, the script provides instructions to add it.