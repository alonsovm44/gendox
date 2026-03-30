# Docgen — End documentation drift for good

[![Release](https://img.shields.io/github/v/release/alonsovm44/docgen?color=blue&style=flat-square)](https://github.com/alonsovm44/docgen/releases)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white&style=flat-square)]()
[![Local AI: Ollama](https://img.shields.io/badge/Local_AI-Ollama-black?logo=ollama&style=flat-square)]()
[![License](https://img.shields.io/github/license/alonsovm44/docgen?style=flat-square)]()
[![CI](https://img.shields.io/badge/CI-%20setup%20recommended-lightgrey?style=flat-square)]()  <!-- replace when CI exists -->

![Docgen Demo](assets/demo.gif)

One-liner
Docgen automatically keeps your C++ project docs accurate: store intent/spec next to source, run `docgen update` and only changed files get regenerated. Fast, auditable, and works locally with Ollama or in the cloud.

Why it matters
- Documentation bitrots fast. Docgen treats docs like build artifacts: incremental, reproducible, and verifiable.
- Keep human-readable intent in the repo; generate the implementation docs from those intents with LLM help without manual bookkeeping.
- Works offline with local models (recommended) and supports cloud backends for CI.

Try it now (safe copy-paste)
Note: avoid piping unknown remote scripts into a shell. Prefer downloading and inspecting installer scripts or use prebuilt release assets.

Quick (recommended):
1. Download installer, inspect, run:
```bash
# Download
curl -fsSL -o ./docgen-installer.sh https://github.com/alonsovm44/docgen/releases/latest/download/install.sh
# Inspect: less ./docgen-installer.sh
# Then run:
bash ./docgen-installer.sh
```

2. Initialize a project:
```bash
docgen init
docgen track main.cpp src/
docgen update
```

Or (experienced users who trust the release and want one-liners):
```bash
# Unix/macOS (use only when you trust the source)
curl -fsSL https://github.com/alonsovm44/docgen/releases/latest/download/install.sh | bash
# Windows (PowerShell)
# Download installer.ps1, inspect and then run:
# iwr <url> -OutFile installer.ps1; notepad installer.ps1; powershell -ExecutionPolicy Bypass -File installer.ps1
```

New interactive chat
You can now open an in-terminal chat that uses your project documentation as context:
```bash
docgen query --chat
# or open a project-specific session:
docgen query --chat --project .  # reads .docgen/docs and project context
```
This launches a terminal chat where the AI answers questions about the documented codebase and links back to generated pages.

Core concepts (short)
- Docs-as-Code: generated Markdown lives next to source, tracked in `.docgen/`.
- Incremental builds: file-level hashing avoids re-generating unchanged docs.
- Context-aware (RAG): dependency analysis provides the LLM with the right context for complex APIs.
- Local-first: Ollama support by default; cloud backends optionally supported.
- Single static binary: zero runtime dependencies required.

Docfile — example (store the blueprint for Docgen)
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

Common commands (copy-paste)
- Initialize project:
  docgen init

- Track files/directories:
  docgen track <path>

- Update docs (generate):
  docgen update

- Check status:
  docgen status

- Generate project summary:
  docgen summary

- Dependency graph (DOT):
  docgen graph

- Verify docs are up-to-date (use in CI):
  docgen validate

- Clean deleted/untracked docs:
  docgen clean

- Upgrade docgen (fetch latest release):
  docgen upgrade

- Reset docgen repo (asks confirmation):
  docgen reboot

Config & connection
Run `docgen config` to manage the backend and model:
```bash
docgen config mode local      # default (Ollama)
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

Best practices & CI
- Add `docgen validate` to CI to fail builds if docs are stale.
- Use release artifacts (prebuilt binaries) and publish SHA256 checksums. Avoid piping install scripts directly into shells.
- Make generated docs part of a docs branch or a docs-only PR that runs link-checkers, linters, and security scans.

Security & provenance
- Docgen embeds provenance metadata into generated files: tool version, model-id (or hash), generation timestamp, and container/hash info. This makes generated content auditable in PRs.

Examples & demos
- examples/onboarding: a small module + Docfile showing how new contributors can generate working documentation and runnable examples.
- examples/boilerplate: generate serialization and README for a simple class automatically.

FAQ (short)
Q: Is Docgen rewriting my code?  
A: No. Docgen reads source and generates Markdown docs. It never mutates your source files unless you explicitly run commands that write generated artifacts.

Q: Is it deterministic?  
A: Docgen hashes context and caches outputs. Determinism depends on model behavior. For full reproducibility, pin model + prompt versions and use the cache/artifact files.

Contributing & support
- Please open issues for bugs/feature requests and PRs for examples and docs. See CONTRIBUTING.md and SECURITY.md for reporting sensitive issues.

License
- MIT — see LICENSE file.

Links
- Repo: https://github.com/alonsovm44/docgen
- Whitepaper / design notes: .DOCUMENTATION/paper.md