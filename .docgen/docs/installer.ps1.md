```markdown
# docgen Setup Script Documentation

## Purpose
This PowerShell script automates the setup and compilation of the `docgen` tool. It checks for prerequisites, installs missing dependencies, and compiles the source code using an available C++ compiler.

## Usage
Run the script in a PowerShell environment with administrative privileges if installing dependencies like Ollama. The script handles the following tasks:

1. **Prerequisite Check**: Verifies the presence of `curl` and `ollama`.
2. **Dependency Installation**: Prompts to install Ollama if it's missing.
3. **Compilation**: Compiles `docgen` using either `g++` or `MSVC` (cl) if available.

## Behavior

### Prerequisite Check
- **curl**: Required for API requests. If missing, a warning is displayed, but the script continues.
- **ollama**: If missing, the script prompts the user to install it. If the user agrees (`y`), it downloads and runs the Ollama installer.

### Compilation
- **Compiler Detection**: Automatically detects and uses `g++` (MinGW) or `MSVC` (cl) for compilation.
- **Compilation Flags**:
  - **g++**: Uses `-std=c++17` for C++17 standard compliance.
  - **MSVC**: Uses `/EHsc` (C++ exceptions) and `/std:c++17` for C++17 standard compliance.
- **Output**: Produces `docgen.exe` in the current directory if compilation succeeds.

### Error Handling
- Stops execution on critical errors (e.g., no compiler found).
- Exits with a non-zero status code if compilation fails.

## Example Workflow
1. Run the script.
2. If `ollama` is missing, choose `y` to install it.
3. The script compiles `docgen` using the detected compiler.
4. Upon successful compilation, run `.\docgen.exe` to start the tool.

## Requirements
- PowerShell 5.1 or later.
- Internet connection (for downloading Ollama if needed).
- Administrative privileges (for installing Ollama).

## Notes
- Ensure the `src/main.cpp` file is present in the expected location.
- The script assumes `curl` and `ollama` are in the system's PATH.
```