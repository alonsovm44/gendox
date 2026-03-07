# `docgen` Command-Line Tool Documentation

The `docgen` tool is designed to automate the generation and maintenance of documentation for codebases using AI. Below is a detailed breakdown of its functionality, commands, and usage.

---

## **Initialization (`cmd_init`)**

**Purpose**: Initializes a new documentation repository in the current directory.

**Behavior**:
- Checks if `.docgen/` or `Docfile` already exists. If so, it exits with an error.
- Creates the following:
  - `Docfile`: A configuration file with example sections for tracking, ignoring, and styling documentation.
  - `.docgen/`: A directory to store configuration, documentation, and lock files.
  - `.docgen/docs/`: A subdirectory to store generated documentation.
  - `docgen_config.json`: A configuration file with default settings for local and cloud AI modes.
  - `docgen.lock`: A lock file to track file hashes for incremental updates.

**Usage**:
```bash
docgen init
```

---

## **Configuration Management (`cmd_config`)**

**Purpose**: Manages configuration settings for the `docgen` tool.

**Behavior**:
- Allows setting or viewing configuration keys such as `mode`, `protocol`, `key`, and `model`.
- Validates the configuration file's existence before making changes.
- Updates the `docgen_config.json` file with the new values.

**Usage**:
- View current configuration:
  ```bash
  docgen config see
  ```
- Set a configuration key:
  ```bash
  docgen config <key> <value>
  ```
  Example:
  ```bash
  docgen config mode cloud
  ```

---

## **Tracking and Ignoring Files (`cmd_track_ignore`)**

**Purpose**: Adds files or patterns to the `Track` or `Ignore` sections of the `Docfile`.

**Behavior**:
- Checks if `Docfile` exists. If not, it exits with an error.
- Appends the specified path to the appropriate section (`Track` or `Ignore`).
- Handles comments and ensures the path is added only once.

**Usage**:
- Track a file or pattern:
  ```bash
  docgen track <path>
  ```
- Ignore a file or pattern:
  ```bash
  docgen ignore <path>
  ```

---

## **AI-Powered Documentation Generation (`call_ai`)**

**Purpose**: Generates Markdown documentation for a given file using AI.

**Behavior**:
- Reads the configuration to determine the AI mode (`local` or `cloud`).
- Extracts included files (e.g., `#include` directives) and builds context for the AI.
- Sends a prompt to the AI model, including the code, context, and style guidelines.
- Handles retries and rate limiting for cloud API calls.
- Saves the generated documentation to `.docgen/docs/`.

**Usage**:
- Called internally by `cmd_update`.

---

## **Updating Documentation (`cmd_update`)**

**Purpose**: Updates documentation for tracked files based on changes detected since the last update.

**Behavior**:
- Reads `Docfile` to determine tracked and ignored files.
- Scans the project directory for files matching the tracked patterns.
- Compares file hashes with the lock file to identify changes.
- Calls `call_ai` to generate documentation for updated files.
- Updates the lock file with new hashes.

**Usage**:
```bash
docgen update
```
- Add `--verbose` for detailed logging:
  ```bash
  docgen update --verbose
  ```

---

## **Upgrading the Tool (`cmd_upgrade`)**

**Purpose**: Checks for and applies updates to the `docgen` tool.

**Behavior**:
- Downloads and executes an update script from the GitHub repository.
- Supports Windows (PowerShell) and Unix-like systems (Bash).

**Usage**:
```bash
docgen upgrade
```

---

## **Rebooting the Repository (`cmd_reboot`)**

**Purpose**: Resets the documentation repository by deleting `Docfile` and `.docgen/`.

**Behavior**:
- Prompts the user for confirmation before deleting files.
- Removes the specified directories and files if confirmed.

**Usage**:
```bash
docgen reboot
```

---

## **Sponsoring the Project (`cmd_sponsor`)**

**Purpose**: Opens the GitHub Sponsors page for the project maintainer.

**Behavior**:
- Uses the system's default browser to open the sponsorship page.

**Usage**:
```bash
docgen sponsor
```

---

## **Key Features**
- **Incremental Updates**: Only processes files that have changed since the last update.
- **Context-Aware Documentation**: Includes context from imported files in the AI prompt.
- **Configurable AI Modes**: Supports both local and cloud-based AI models.
- **Pattern Matching**: Uses flexible patterns for tracking and ignoring files.
- **Error Handling**: Retries failed AI requests and handles rate limiting.

---

## **Configuration File (`docgen_config.json`)**

Example structure:
```json
{
    "mode": "local",
    "cloud": {
        "api_key": "your_api_key",
        "api_url": "https://apifreellm.com/api/v1/chat",
        "model_id": "qwen2.5-coder:7b",
        "protocol": "simple"
    },
    "local": {
        "api_url": "http://localhost:11434/api/generate",
        "model_id": "qwen2.5-coder:3b"
    }
}
```

---

## **Docfile Structure**

Example `Docfile`:
```plaintext
Track:
    # Example: *.cpp
    src/*.cpp

Ignore:
    # Example: secret.cpp
    src/secret.cpp

Style:
    # Example: no emojis
    - Use concise language
    - Avoid emojis
```

---

## **Dependencies**
- C++17 or later.
- `nlohmann/json` for JSON parsing.
- `std::filesystem` for file and directory operations.
- `curl` for HTTP requests (used in `exec_curl`).

---

This documentation provides a comprehensive overview of the `docgen` tool's functionality and usage. For further details, refer to the source code and inline comments.