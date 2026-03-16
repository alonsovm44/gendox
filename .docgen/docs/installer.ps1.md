# docgen Setup Script Documentation

## Purpose
This PowerShell script automates the setup and compilation of the `docgen` tool. It checks for prerequisites, installs missing dependencies, clones the source code if necessary, and compiles the source code using an available C++ compiler.

## Usage
Run the script in a PowerShell environment with administrative privileges if installing dependencies like Ollama. The script handles the following tasks:

1. **Prerequisite Check**: Verifies the presence of `curl`, `git`, and `ollama`.
2. **Source Code Retrieval**: Clones the `docgen` repository if the source code is not found locally.
3. **Dependency Installation**: Prompts to install Ollama and a C++ compiler (MinGW) if they're missing.
4. **Compilation**: Compiles `docgen` using either `g++`, `clang++`, or `MSVC` (cl) if available.

## Behavior

### Prerequisite Check
- **curl**: Required for API requests. If missing, a warning is displayed, but the script continues.
- **git**: Required to clone the source code if not found locally. If missing, the script exits with an error.
- **ollama**: If missing, the script prompts the user to install it. If the user agrees (`y`), it downloads and runs the Ollama installer.

### Source Code Retrieval
- If `src/main.cpp` is not found, the script clones the `docgen` repository from GitHub into a temporary directory.

### Compiler Installation
- If no C++ compiler is found, the script downloads and installs a portable version of MinGW (w64devkit) and adds it to the system's PATH.

### Compilation
- **Compiler Detection**: Automatically detects and uses `g++`, `clang++`, or `MSVC` (cl) for compilation, with a preference for `g++`.
- **Compilation Flags**:
  - **g++/clang++**: Uses `-std=c++17` for C++17 standard compliance.
  - **MSVC**: Uses `/EHsc` (C++ exceptions) and `/std:c++17` for C++17 standard compliance.
- **Output**: Produces `docgen.exe` and installs it in the `%LOCALAPPDATA%\docgen\bin` directory, adding it to the user's PATH.

### Error Handling
- Stops execution on critical errors (e.g., no compiler found after installation).
- Exits with a non-zero status code if compilation fails.

## Example Workflow
1. Run the script.
2. If the source code is not found locally, it is cloned from GitHub.
3. If `ollama` is missing, choose `y` to install it.
4. If no compiler is found, MinGW is installed.
5. The script compiles `docgen` using the detected compiler.
6. Upon successful compilation, `docgen` is installed and added to the user's PATH.

## Requirements
- PowerShell 5.1 or later.
- Internet connection (for downloading dependencies and source code).
- Administrative privileges (for installing Ollama and modifying the PATH).

## Notes
- The script cleans up temporary directories after completion.
- Ensure an active internet connection for downloading dependencies and source code.
- The compiled `docgen.exe` is installed in `%LOCALAPPDATA%\docgen\bin`.