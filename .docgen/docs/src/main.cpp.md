# docgen Command-Line Tool Documentation

## Overview
The `docgen` tool is a command-line utility designed to manage documentation-related tasks. It provides a set of commands to initialize, configure, track, and update documentation projects. The tool also includes utility commands for version checking, upgrading, and accessing external resources.

## Usage
The tool is invoked via the command line with the following syntax:
```bash
docgen <command> [args...]
```

## Commands

### Version
**Purpose:** Display the current version of the `docgen` tool.  
**Usage:**  
```bash
docgen version
```  
**Aliases:** `-version`, `--version`  
**Behavior:** Prints the version string (`docgen version 0.0.1`) and exits successfully.

### Init
**Purpose:** Initialize a new documentation project.  
**Usage:**  
```bash
docgen init
```  
**Behavior:** Calls the `cmd_init()` function to set up the necessary project structure.

### Config
**Purpose:** Manage configuration settings for the documentation project.  
**Usage:**  
```bash
docgen config [args...]
```  
**Behavior:** Calls the `cmd_config()` function, passing all arguments for configuration management.

### Track
**Purpose:** Add a file or directory to the documentation tracking system.  
**Usage:**  
```bash
docgen track <path>
```  
**Behavior:** Requires a `<path>` argument. Calls `cmd_track_ignore("track", <path>)` to mark the specified path for tracking.

### Ignore
**Purpose:** Exclude a file or directory from the documentation tracking system.  
**Usage:**  
```bash
docgen ignore <path>
```  
**Behavior:** Requires a `<path>` argument. Calls `cmd_track_ignore("ignore", <path>)` to mark the specified path for exclusion.

### Update
**Purpose:** Update the documentation based on tracked changes.  
**Usage:**  
```bash
docgen update [-v | --verbose]
```  
**Behavior:** Optionally accepts a verbose flag (`-v` or `--verbose`). Calls `cmd_update(verbose)` to perform updates, with verbose output if enabled.

### Upgrade
**Purpose:** Upgrade the `docgen` tool to the latest version.  
**Usage:**  
```bash
docgen upgrade
```  
**Behavior:** Calls `cmd_upgrade()` to handle the upgrade process.

### Reboot
**Purpose:** Reboot the documentation system (specific implementation details not provided).  
**Usage:**  
```bash
docgen reboot
```  
**Behavior:** Calls `cmd_reboot()` to perform the reboot action.

### Sponsor
**Purpose:** Access sponsorship information or resources.  
**Usage:**  
```bash
docgen sponsor
```  
**Behavior:** Calls `cmd_sponsor()` to handle sponsorship-related actions.

### Get-Key
**Purpose:** Open a browser to retrieve an API key from `https://apifreellm.com`.  
**Usage:**  
```bash
docgen get-key
```  
**Behavior:** Uses platform-specific commands (`start`, `open`, or `xdg-open`) to open the specified URL in the default browser.

## Error Handling
- If no command is provided, the tool prints usage instructions and exits with a failure code (`1`).  
- Unknown commands result in an "Unknown command" message and exit with a failure code (`1`).  
- Commands requiring arguments (e.g., `track`, `ignore`) print usage instructions and exit with a failure code (`1`) if arguments are missing.

## Dependencies
The tool relies on functions defined in `core.hpp` (`cmd_init()`, `cmd_config()`, `cmd_track_ignore()`, etc.) for command implementations. Platform-specific commands are used for opening URLs in the `get-key` command.