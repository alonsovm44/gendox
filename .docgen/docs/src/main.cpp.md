Here is the updated documentation with **only the sections that changed**, preserving the existing structure and wording wherever possible.

---

# docgen Command-Line Tool Documentation

## Overview
`docgen` is a command-line tool designed to manage documentation generation and related tasks. It provides a set of subcommands to initialize, configure, track, validate, clean, and update documentation projects. The tool also includes utility commands for version checking, system management, and external resource access.

## Usage
The tool is invoked via the command line with the following syntax:

```bash
docgen <command> [args...]
```

If no command is provided, the tool displays the help message and exits successfully.

## Commands

### Version
**Purpose:** Display the version of the `docgen` tool.  
**Usage:**  
```bash
docgen version
```  
or  
```bash
docgen -version
```  
or  
```bash
docgen --version
```  
**Behavior:** Prints the current version (`0.1.0`) and exits successfully.

### Help
**Purpose:** Display the help message with available commands.  
**Usage:**  
```bash
docgen help
```  
or  
```bash
docgen -h
```  
or  
```bash
docgen --help
```  
**Behavior:** Prints the help message with a list of available commands and exits successfully.

### Init
**Purpose:** Initialize a new documentation project.  
**Usage:**  
```bash
docgen init
```  
**Behavior:** Calls the `cmd_init()` function to set up the project structure.

### Config
**Purpose:** Manage configuration settings for the documentation project (AI mode, keys, models).  
**Usage:**  
```bash
docgen config [args...]
```  
**Behavior:** Calls `cmd_config(argc, argv)` with all provided arguments.

### Track
**Purpose:** Add a file or directory to the documentation tracking system.  
**Usage:**  
```bash
docgen track <path>
```  
**Behavior:** Requires a `<path>` argument. Calls `cmd_track_ignore("track", <path>)`.

### Ignore
**Purpose:** Exclude a file or directory from the documentation tracking system.  
**Usage:**  
```bash
docgen ignore <path>
```  
**Behavior:** Requires a `<path>` argument. Calls `cmd_track_ignore("ignore", <path>)`.

### Update
**Purpose:** Generate or update documentation for tracked files.  
**Usage:**  
```bash
docgen update [-v | --verbose]
```  
**Behavior:** Accepts an optional verbose flag. Calls `cmd_update(verbose)`.

### Status
**Purpose:** Show the status of tracked files (new or modified).  
**Usage:**  
```bash
docgen status
```  
**Behavior:** Calls `cmd_status()`.

### Summary
**Purpose:** Generate a summary of the current documentation status.  
**Usage:**  
```bash
docgen summary
```  
**Behavior:** Calls `cmd_summary()`.

### Validate
**Purpose:** Check whether documentation is up-to-date with tracked files.  
**Usage:**  
```bash
docgen validate
```  
**Behavior:** Calls `cmd_validate()` to verify documentation freshness.

### Clean
**Purpose:** Remove documentation for files that are no longer tracked.  
**Usage:**  
```bash
docgen clean
```  
**Behavior:** Calls `cmd_clean()` to delete documentation associated with untracked files.

### Upgrade
**Purpose:** Upgrade the `docgen` tool to the latest version.  
**Usage:**  
```bash
docgen upgrade
```  
**Behavior:** Calls `cmd_upgrade()`.

### Reboot
**Purpose:** Reset the documentation repository (deletes `.docgen/`).  
**Usage:**  
```bash
docgen reboot
```  
**Behavior:** Calls `cmd_reboot()`.

### Sponsor
**Purpose:** Open the GitHub Sponsors page.  
**Usage:**  
```bash
docgen sponsor
```  
**Behavior:** Calls `cmd_sponsor()`.

### Get-Key
**Purpose:** Open a browser to retrieve an API key from `https://apifreellm.com`.  
**Usage:**  
```bash
docgen get-key
```  
**Behavior:** Uses platform-specific commands (`start`, `open`, or `xdg-open`) to open the URL.

## Error Handling
- If an unknown command is provided, the tool prints an error message and exits with a non-zero status code.
- Commands requiring arguments (such as `track` and `ignore`) display usage instructions and exit with an error if arguments are missing.

## Dependencies
The tool relies on the `core.hpp` header for function definitions (`cmd_init()`, `cmd_config()`, `cmd_validate()`, `cmd_clean()`, etc.). Platform-specific commands are used for opening URLs in a browser.

---

If you'd like, I can also generate a diff-style summary showing exactly what changed.