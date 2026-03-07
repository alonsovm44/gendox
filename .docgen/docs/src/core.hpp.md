# `docgen` Command-Line Tool Documentation

## Overview
The `docgen` tool is a command-line utility designed to automate the generation and maintenance of documentation for software projects. It leverages AI models to create comprehensive Markdown documentation for source code files, tracks changes, and manages project configuration.

---

## Commands

### `docgen init`
**Purpose:** Initializes a new documentation repository in the current directory.  
**Behavior:**
- Creates a `.docgen` directory and a `Docfile` for configuration.
- Prompts for a project name.
- Sets up initial configuration files (`docgen_config.json`, `docgen.lock`, `tree.json`).
- Creates a `docs` directory for generated documentation.
- Initializes `docgen_config.json` with default settings for local and cloud AI models.

**Example Usage:**
```bash
docgen init
```

---

### `docgen config`
**Purpose:** Manages the configuration settings for the `docgen` tool.  
**Behavior:**
- Allows setting or viewing configuration keys (`mode`, `protocol`, `key`, `model`).
- Supports checking the connection to the AI model.
- Updates specific configuration fields like `cloud.api_key`, `cloud.protocol`, and `model_id` for both local and cloud modes.
- Includes enhanced error handling for API key issues and rate limits during connection checks.

**Usage:**
- **Set a configuration key:**  
  ```bash
  docgen config <key> <value>
  ```
  Example:  
  ```bash
  docgen config mode cloud
  ```

- **View current configuration:**  
  ```bash
  docgen config see
  ```

- **Check AI model connection:**  
  ```bash
  docgen config check
  ```

---

### `docgen track/ignore`
**Purpose:** Adds files or patterns to the `Track` or `Ignore` sections of the `Docfile`.  
**Behavior:**
- Appends the specified path or pattern to the appropriate section in `Docfile`.
- Handles comments and pattern syntax consistently.

**Example Usage:**
```bash
docgen track src/*.cpp
docgen ignore tests/*.cpp
```

---

### `docgen update`
**Purpose:** Updates documentation for tracked files.  
**Behavior:**
- Scans tracked files, compares hashes with the lockfile, and generates documentation for changed files.
- Uses AI to generate Markdown documentation, incorporating context from included files.
- Updates `tree.json` and `docgen.lock` with new file information.
- Handles rate limits and retries for cloud AI models.
- Includes RAG (Retrieval-Augmented Generation) for improved context handling.

**Example Usage:**
```bash
docgen update
```

---

### `docgen summary`
**Purpose:** Generates a summary of the project's documentation.  
**Behavior:**
- Reads `tree.json` and creates a `SUMMARY.md` file with file summaries and links to full documentation.
- Sorts files alphabetically and includes the last update timestamp.

**Example Usage:**
```bash
docgen summary
```

---

### `docgen status`
**Purpose:** Shows the status of tracked files.  
**Behavior:**
- Compares file hashes with the lockfile and indicates new or modified files.
- Uses consistent pattern matching and file hashing.

**Example Usage:**
```bash
docgen status
```

---

### `docgen clean`
**Purpose:** Cleans up outdated documentation files.  
**Behavior:**
- Removes documentation for untracked files and updates `tree.json` and `docgen.lock`.

**Example Usage:**
```bash
docgen clean
```

---

### `docgen validate`
**Purpose:** Validates the consistency of documentation.  
**Behavior:**
- Checks for untracked files, outdated documentation, and missing files.

**Example Usage:**
```bash
docgen validate
```

---

### `docgen upgrade`
**Purpose:** Checks for and applies updates to the `docgen` tool.  
**Behavior:**
- Downloads and executes the update script from the GitHub repository.

**Example Usage:**
```bash
docgen upgrade
```

---

### `docgen reboot`
**Purpose:** Resets the `docgen` repository.  
**Behavior:**
- Deletes the `Docfile` and `.docgen` directory after user confirmation.

**Example Usage:**
```bash
docgen reboot
```

---

### `docgen sponsor`
**Purpose:** Opens the GitHub Sponsors page for the project maintainer.  
**Behavior:**
- Launches the default browser to the sponsor page.

**Example Usage:**
```bash
docgen sponsor
```

---

## Configuration
The `docgen` tool uses a JSON configuration file (`docgen_config.json`) to manage settings for AI models and project behavior. Key configuration options include:
- **Mode:** `local` or `cloud` (determines the AI model endpoint).
- **API Key:** Required for cloud mode.
- **Model ID:** Specifies the AI model to use.
- **Protocol:** Defines the API protocol (e.g., `simple`, `openai`, `google`).
- **Cloud Settings:** Includes `api_url`, `api_key`, `model_id`, and `protocol`.
- **Local Settings:** Includes `api_url` and `model_id`.

---

## File Structure
- **`.docgen/`**: Contains configuration and generated files.
  - `docgen_config.json`: Configuration settings.
  - `docgen.lock`: Tracks file hashes for incremental updates.
  - `tree.json`: Stores metadata about tracked files.
  - `docs/`: Directory for generated documentation.
- **`Docfile`**: Defines tracking, ignoring, and styling rules.

---

## AI Integration
The tool integrates with AI models to generate documentation. It supports:
- **Local Models:** Runs on a local server (e.g., Ollama).
- **Cloud Models:** Uses remote APIs (e.g., OpenAI, Google).
- **Contextual Documentation:** Incorporates context from included files to improve documentation accuracy.
- **Rate Limit Handling:** Retries requests with exponential backoff for cloud models.
- **RAG (Retrieval-Augmented Generation):** Enhances context handling by including relevant code snippets.

---

## Usage Workflow
1. **Initialize:** Run `docgen init` to set up the repository.
2. **Configure:** Use `docgen config` to set up AI model settings.
3. **Track Files:** Add files to `Track` in `Docfile`.
4. **Generate Docs:** Run `docgen update` to generate documentation.
5. **Summarize:** Use `docgen summary` to create a project summary.
6. **Check Status:** Use `docgen status` to view file changes.
7. **Clean Up:** Use `docgen clean` to remove outdated documentation.
8. **Validate:** Use `docgen validate` to ensure documentation consistency.

---

## Error Handling
- **Configuration Errors:** Clear error messages for missing or invalid configurations.
- **AI Errors:** Retries on failures and provides detailed error messages.
- **File Conflicts:** Warns if `.docgen/` or `Docfile` already exists during initialization.
- **Rate Limits:** Handles API rate limits with retries and backoff.
- **Validation Errors:** Reports untracked files, outdated documentation, and missing files.

---

## Dependencies
- **C++ Standard Library:** For file system operations, threading, and JSON handling.
- **External Tools:** Relies on `curl` for HTTP requests and AI model communication.

---

## Limitations
- **Prompt Size:** Limited by the AI model's context window (mitigated by truncating context).
- **File Types:** Only processes text files.
- **Platform Dependency:** Some commands (e.g., `upgrade`, `sponsor`) use platform-specific commands.

---

This documentation provides a comprehensive overview of the `docgen` tool's functionality, usage, and behavior. For further details, refer to the source code and inline comments.