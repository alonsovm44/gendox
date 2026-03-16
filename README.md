# Docgen 

![GitHub Release](https://img.shields.io/github/v/release/alonsovm44/docgen?color=blue&style=flat-square)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white&style=flat-square)
![Ollama](https://img.shields.io/badge/Local_AI-Ollama-black?logo=ollama&style=flat-square)
![License](https://img.shields.io/github/license/alonsovm44/docgen?style=flat-square)

**Solve documentation hell for good.**

Docgen is a lightweight, C++ CLI tool that automates software documentation using Large Language Models (LLMs). It treats documentation generation as a build step, tracking your source code and incrementally updating documentation only when files change.

## Why this solves "Documentation Hell"?
Because you no longer have to bother writing docs yourself, just run `docgen update` and you are good to go. 

## Features

*   **Docs-as-Code**: Generates Markdown documentation that lives right next to your source code.
*   **Smart Incremental Builds**: Uses content hashing to only regenerate docs for modified files, saving time and API credits.
*   **Context-Aware (RAG)**: Automatically analyzes dependencies to provide relevant context to the AI, ensuring accurate documentation for complex projects.
*   **Flexible Backend**: 
    *   **Cloud**: Supports OpenAI, Google Gemini, and compatible APIs.
    *   **Local**: Works fully offline with Ollama (default).
*   **Zero Dependencies**: Compiled as a single static binary.

## Getting started
## Quick Install

### Windows (PowerShell)
```powershell
irm https://raw.githubusercontent.com/alonsovm44/docgen/master/installer.ps1 | iex
```
### Linux / Mac
```bash
curl -fsSL https://raw.githubusercontent.com/alonsovm44/docgen/master/installer.sh | bash
```

After installation run
```bash
docgen init
```
This creates your `Docfile` and your `.docgen` repo. Using the Docfile is easy. 
Run `docgen track <path>` this adds the file/directory to your Docfile. 
Then run `docgen update` to generate your tracked files documentation. Once generated only files with changes will be updated. 
You can also run `docgen ignore` to indicate to Docgen which files or directories to ignore.

**Example Docfile**
```Docfile
Track:
# This is a comment
    main.cpp
    src/
Ignore:
    src/secret.cpp

Style:
    # these get passed to the AI as parameters when making docs
    -dont use emojis
    -be professional
    -be concise
```


## Other commands

### Status
Check which files have changed since the last update.
```bash
docgen status
```

### Summary
Generate a `SUMMARY.md` file with a high-level overview of your project.
```bash
docgen summary
```

### Graph
Generate a dependency graph in DOT format (Graphviz) at `.docgen/graph.dot`.
```bash
docgen graph
```

### Clean
Remove documentation for files that are no longer tracked.
```bash
docgen clean
```

### Validate
Verify if documentation is up-to-date (useful for CI/CD).
```bash
docgen validate
```

### Upgrade
Update docgen to the latest version.
```bash
docgen upgrade
```

### Reboot
Run this command to reset your documentation repository (asks for confirmation)
```bash
docgen reboot
```
### config
Docgen comes with a config.json file within your `./docgen` folder. In it you can configure A.I parameters

You can also run 
```shell
$ docgen config

Usage: docgen config <key> <value>
Keys: mode, protocol, key, model
```
#### Check Connection
To verify if Docgen can connect to the configured AI provider:
```bash
docgen config check
```

#### Switching Modes
Docgen supports two modes: `local` (default, uses Ollama) and `cloud` (uses external APIs).

**1. Local Mode (Ollama)**
This is the default. It requires Ollama running locally.
```bash
# Set mode to local
docgen config mode local

# Set the model (e.g., qwen2.5-coder:7b, llama3, mistral)
docgen config model qwen2.5-coder:7b
```

**2. Cloud Mode (OpenAI, Google, etc.)**
Use this to connect to OpenAI, Google Gemini, or compatible APIs.

```bash
# Set mode to cloud
docgen config mode cloud

# Set your API key
docgen config key sk-your-api-key-here

# Set the model ID (e.g., gpt-4o, gemini-1.5-flash)
docgen config model gpt-4o

# Set the protocol (openai, google, or simple)
# 'openai' is standard for most providers including DeepSeek, Groq, etc.
docgen config protocol openai
```

#### Configuration Keys reference

| Key | Description | Values |
| :--- | :--- | :--- |
| `mode` | Determines the backend to use. | `local`, `cloud` |
| `key` | API Key for cloud providers. | Your API Key |
| `model` | The model identifier to use. | e.g., `gpt-4o`, `llama3` |
| `protocol` | The API format to use. | `openai`, `google`, `simple` |