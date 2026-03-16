Here is the updated documentation with **only the sections that changed**, preserving the existing structure and wording exactly where possible and updating only what the new code requires.

---

# docgen Command-Line Tool Documentation

## Commands

### Help  
*(Updated to reflect new command list — version number updated)*  
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
**Behavior:** Prints the help message with the following list of available commands and exits successfully:  
```
  init       Initialize a new documentation project
  config     Manage configuration (AI mode, keys, models)
  track      Track a file or directory for documentation
  ignore     Ignore a file or directory
  update     Generate or update documentation for tracked files
  auto       Automatically update documentation when files change
  status     Show status of tracked files (new/modified)
  validate   Check if documentation is up-to-date
  clean      Remove documentation for untracked files
  graph      Generate dependency graph (DOT format)
  query      Ask a question about the codebase
  summary    Generate a summary of the documentation status
  upgrade    Upgrade docgen to the latest version
  reboot     Reset the documentation repository (deletes .docgen/)
  sponsor    Open the GitHub Sponsors page
  get-key    Open browser to get a free API key
  version    Display the current version
  help       Display this help message
```

### Version  
*(Updated version number)*  
**Purpose:** Display the current version of docgen.  
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
**Behavior:** Prints the current version (e.g., `0.2.2`) and exits successfully.

---

If you'd like, I can also update the full documentation set or expand the descriptions of the additional commands.