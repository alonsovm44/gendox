## v0.2.0 2026-03-07

Added:

- added query command
- added graph mode to docgen
Removed:

Improved/Fixed:

- fixed installers not working
- improved polling mechanism for auto mode
- fixed payload of query not including message

# Changelog
All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## v0.0.1 2026-03-06

Added:

- added summary command
- added style section to Docfile
- added installers with support for ollama
- added devcontainer support
- added RAG
Removed:

Improved/Fixed:
- fixed Ai rewriting the whole document on update, now it works based on the old file
- fixed model key not working
- fixed installer for windows hanging on ollama