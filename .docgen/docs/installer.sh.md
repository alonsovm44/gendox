# docgen Build Script Documentation

## Purpose
This script automates the setup and compilation of the `docgen` tool. It checks for prerequisites, installs missing dependencies (if allowed), and compiles the source code using an available C++ compiler.

## Behavior

### Prerequisites Check
1. **curl**:  
   - Verifies if `curl` is installed.  
   - Exits with an error if missing, as `docgen` requires `curl` for API requests and the script needs it to download Ollama.  

2. **git**:  
   - Checks if `git` is installed if `src/main.cpp` is not found locally.  
   - Exits with an error if `git` is missing, as it is required to clone the source code.  

3. **Ollama**:  
   - Checks for the presence of `ollama`.  
   - If missing, prompts the user to install it via an interactive prompt.  
   - Installation is performed using the official Ollama installation script if the user consents.  

### Source Code Retrieval
- If `src/main.cpp` is not found locally, the script clones the `docgen` repository from GitHub into a temporary directory.  

### Compilation
1. **Compiler Detection and Installation**:  
   - Searches for `g++` or `clang++` in the system.  
   - If no compiler is found, attempts to install `g++` using the system's package manager.  
   - Exits with an error if no compiler can be installed.  

2. **Build Process**:  
   - Compiles `src/main.cpp` with C++17 standard support.  
   - Outputs the executable as `docgen`.  

3. **Installation**:  
   - Moves the compiled `docgen` executable to `$HOME/.local/bin`.  
   - Advises the user to add this directory to their `PATH` if it's not already included.  

4. **Cleanup**:  
   - If the repository was cloned, the temporary directory is deleted after compilation.  

## Usage
1. **Run the Script**:  
   Execute the script in a terminal:  
   ```bash
   ./build.sh
   ```

2. **Interactive Prompts**:  
   - If `ollama` is missing, respond with `y` or `n` to install or skip installation.  

3. **Post-Build**:  
   - Upon successful compilation, run the tool using:  
     ```bash
     docgen
     ```

## Error Handling
- Exits with a non-zero status if:  
  - `curl` or `git` is not found.  
  - No C++ compiler is found and cannot be installed.  
  - The build process fails.  

## Dependencies
- **curl**: Required for API requests and downloading Ollama.  
- **git**: Required if `src/main.cpp` is not found locally.  
- **Ollama**: Optional but recommended for full functionality.  
- **g++ or clang++**: Required for compiling the source code.  

## Notes
- If `src/main.cpp` is not present, the script clones the repository from GitHub.  
- The compiled `docgen` executable is installed in `$HOME/.local/bin`.  
- Ensure appropriate permissions for executing the script and installing dependencies.