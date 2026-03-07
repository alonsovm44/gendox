```markdown
# docgen Build Script Documentation

## Purpose
This script automates the setup and compilation of the `docgen` tool. It checks for prerequisites, installs missing dependencies (if allowed), and compiles the source code using an available C++ compiler.

## Behavior

### Prerequisites Check
1. **curl**:  
   - Verifies if `curl` is installed.  
   - Issues a warning if missing, as `docgen` requires `curl` for API requests.  

2. **Ollama**:  
   - Checks for the presence of `ollama`.  
   - If missing, prompts the user to install it via an interactive prompt.  
   - Installation is performed using the official Ollama installation script if the user consents.  

### Compilation
1. **Compiler Detection**:  
   - Searches for `g++` or `clang++` in the system.  
   - Uses the first available compiler to build `docgen`.  

2. **Build Process**:  
   - Compiles `src/main.cpp` with C++17 standard support.  
   - Outputs the executable as `docgen`.  

3. **Build Verification**:  
   - Confirms successful compilation by checking for the `docgen` executable.  
   - Exits with an error if the build fails.  

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
     ./docgen
     ```

## Error Handling
- Exits with a non-zero status if:  
  - No C++ compiler is found.  
  - The build process fails.  

## Dependencies
- **curl**: Required for API requests.  
- **Ollama**: Optional but recommended for full functionality.  
- **g++ or clang++**: Required for compiling the source code.  

## Notes
- The script assumes `src/main.cpp` exists in the same directory.  
- Interactive prompts are handled differently based on terminal capabilities.  
- Ensure appropriate permissions for executing the script and installing dependencies.
```