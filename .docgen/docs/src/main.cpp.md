Here is the updated documentation with **only the sections that changed**, preserving the existing structure and wording exactly where possible:

---

# docgen Command-Line Tool Documentation

## Commands

### Graph  
*(No changes)*  
**Purpose:** Generate a dependency graph of the project in DOT format.  
**Usage:**  
```bash
docgen graph
```  
**Behavior:** Calls `cmd_graph()` to produce a DOT‑formatted dependency graph.

### Help  
*(Updated to reflect new command list)*  
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

### Get-Key  
*(No changes)*  
**Purpose:** Open a browser to retrieve an API key from `https://apifreellm.com`.  
**Usage:**  
```bash
docgen get-key
```  
**Behavior:** Uses platform‑specific commands (`start`, `open`, or `xdg-open`) to open the URL.

### Version  
*(New command)*  
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
**Behavior:** Prints the current version (e.g., `0.2.0`) and exits successfully.

---

**Additional Commands Added (Brief Overview):**  
The following commands were added to the tool but are not detailed in the original documentation structure:  
- `init`  
- `config`  
- `track`  
- `ignore`  
- `update`  
- `auto`  
- `status`  
- `validate`  
- `clean`  
- `query`  
- `summary`  
- `upgrade`  
- `reboot`  
- `sponsor`  

Let me know if you'd like me to expand on these new commands!