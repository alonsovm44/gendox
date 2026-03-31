# Docgen — End documentation drift for good

[![Release](https://img.shields.io/github/v/release/alonsovm44/docgen?color=blue&style=flat-square)](https://github.com/alonsovm44/docgen/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white&style=flat-square)]()
[![Local AI: Ollama](https://img.shields.io/badge/Local_AI-Ollama-black?logo=ollama&style=flat-square)]()
![Platforms](https://img.shields.io/badge/platforms-Windows%20|%20Linux%20|%20macOS-lightgrey)
[![AI Powered](https://img.shields.io/badge/AI-Powered-blue?style=flat-square)]()

| Basic Workflow | Interactive Chat | Auto-Update |
|:---:|:---:|:---:|
| ![Docgen Demo](assets/demo.gif) | ![Docgen Chat Demo](assets/querydemo.gif) | ![Docgen Auto Demo](assets/auto.gif) |

Docgen automatically keeps your project docs accurate: store intent/spec next to source, run `docgen update` and only changed files get regenerated. Fast, auditable, and works locally with Ollama or in the cloud.

## Why it matters TO YOU NOW
Documentation bitrots fast and it is a pain to maintain, you spend all day updating it, and after one hour of work the docs are a lie. You don't have to deal with that anymore. Just open a terminal on your repo and run `docgen auto` and docs will update in real time as you work. No smoke, no hacks. 


### Stay in the Flow with `docgen auto`

Context switching kills productivity. You write code, then you have to stop, open a `README.md` or a Confluence page, and manually describe your changes. By the time you're done, you've lost your train of thought.

With `docgen auto`, your documentation evolves *with* your code. No more post-sprint "doc-fixing" days. No more stale `README`s. Just accurate, up-to-date documentation, always.

`docgen auto` solves this. Run it in a terminal and forget about it. As you write code, `docgen` watches for file changes and regenerates the relevant documentation *in the background, in real-time*. Your documentation becomes a direct, effortless artifact of your work, not a separate chore.

## The manual workflow
Updating manually is not difficult either. You already know this workflow, it is easy to grasp.
Go to your repo and initialize a project:
```bash
docgen init #This will create a .docgen docs repo and your Docfile
docgen config see # config file of your project, you only do this once.
docgen track main.cpp src/ libs/ utils.hpp ... # add the files or directories you want to track
docgen ignore vars.env node_modules/ __pycache__ # add files and directories you want to ignore
docgen update # this updates tracked files
```
That's it. Docs are updated.

## Try it now (safe copy-paste)
> Note: I get that you may be uncomfortable with piping unknown remote scripts into a shell, you can audit the installer `installer.sh/.ps1` yourself and see it is safe. Otherwise you can use `shellcheck installer.sh`.

- Quick (recommended):

**Linux / macOS:**
Download installer, inspect, run:
```bash
# Download
curl -fsSL https://raw.githubusercontent.com/alonsovm44/docgen/master/installer.sh| bash

```
For windows
```bash
#download and install
irm https://raw.githubusercontent.com/alonsovm44/docgen/master/installer.ps1 | iex
```

## Interactive chat
Good for onboarding.
```bash
docgen query "how does auth work in this project?"
# or you can open a chat session
docgen query --chat
# or open a project-specific session:
docgen query --chat --project .  # reads .docgen/docs and project context
```
This launches a terminal chat where the AI answers questions about the documented codebase and links back to generated pages.

## Core concepts (short)
- Docs-as-Code: generated Markdown lives next to source, tracked in `.docgen/`.
- Incremental builds: file-level hashing avoids re-generating unchanged docs.
- Context-aware (RAG): dependency analysis provides the LLM with the right context for complex APIs.
- Local-first: Ollama support by default; cloud backends optionally supported.
- Single static binary: zero runtime dependencies required.

## Docfile — example (store the blueprint for Docgen)
```Docfile
Track:
    main.cpp
    src/
Ignore:
    src/secret.cpp

Style:
    -dont use emojis
    -be professional
    -be concise
```

## Common commands (copy-paste)
- Initialize project:
  `docgen init`

- Watch for changes, update docs in real time as you work:
 ` docgen auto`

- Query the AI bout the codebase, good for onboarding
`docgen query "query string"`

- Track files/directories:
  `docgen track <path>`

- Update docs (generate):
  `docgen update`

- Check status (like Git status):
  `docgen status`

- Generate project summary:
 ` docgen summary`

- Dependency graph (DOT):
  `docgen graph`

- Verify docs are up-to-date (use in CI):
  `docgen validate`

- Clean deleted/untracked docs:
  `docgen clean`

- Reset docgen repo (asks confirmation):
  `docgen reboot`

## Config & connection
Run `docgen config` to manage the backend and model:
```bash
docgen config mode local    # default (Ollama)
docgen config model qwen2.5-coder:7b
docgen config mode cloud
docgen config protocol openai
docgen config key <YOUR_API_KEY>
docgen config check           # verify connection
```

Configuration keys quick reference

| Key | Description | Example |
|---|---:|---|
| mode | Backend mode | `local` / `cloud` |
| protocol | API format | `openai` / `google` / `simple` |
| key | Cloud API key | `sk-...` |
| model | Model identifier | `gpt-4o`, `qwen2.5-coder:7b` |


## FAQ (short)
Q: Is Docgen rewriting my code?  
A: No. Docgen reads source and generates Markdown docs. It never mutates your source files unless you explicitly run commands that write generated artifacts.

Q: Is it deterministic?  
A: Docgen hashes context and caches outputs. Determinism depends on model behavior. For full reproducibility, pin model + prompt versions and use the cache/artifact files. Docgen reuses docs so they are not rewritten from scratch everytime a tracked file is modified. It is incremental.

## Contributing & support
- Please open issues for bugs/feature requests and PRs for examples and docs. See CONTRIBUTING.md and SECURITY.md for reporting sensitive issues.

## License
- MIT — see LICENSE file.