#!/bin/bash
set -e

echo "Starting gendox installation..."

# Function to install missing packages
install_pkg() {
    local pkg=$1
    echo "Attempting to install $pkg..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update && sudo apt-get install -y "$pkg"
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y "$pkg"
    elif command -v yum &> /dev/null; then
        sudo yum install -y "$pkg"
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --noconfirm "$pkg"
    elif command -v zypper &> /dev/null; then
        sudo zypper install -y "$pkg"
    elif command -v brew &> /dev/null; then
        brew install "$pkg"
    else
        echo "Error: Could not find a supported package manager to install $pkg."
        echo "Please install $pkg manually and run this script again."
        exit 1
    fi
}

# Check for core dependencies
for cmd in curl tar grep; do
    if ! command -v "$cmd" &> /dev/null; then
        install_pkg "$cmd"
    fi
done

# Ask to install Ollama if not found
if ! command -v ollama &> /dev/null; then
    echo "Ollama is not installed. It is highly recommended for local AI processing."
    read -p "Would you like to install Ollama now? (y/n): " install_ollama
    if [[ "$install_ollama" =~ ^[Yy]$ ]]; then
        echo "Installing Ollama..."
        curl -fsSL https://ollama.com/install.sh | sh
    else
        echo "Skipping Ollama installation."
    fi
else
    echo "Ollama is already installed."
fi

INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

# Determine OS and Architecture
OS="$(uname -s | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"
if [[ "$ARCH" == "x86_64" ]]; then ARCH="amd64"; fi
if [[ "$ARCH" == "aarch64" || "$ARCH" == "arm64" ]]; then ARCH="arm64"; fi

echo "Fetching latest release information for gendox..."
LATEST_RELEASE_URL="https://api.github.com/repos/alonsovm44/gendox/releases/latest"
DOWNLOAD_URL=$(curl -s "$LATEST_RELEASE_URL" | grep "browser_download_url" | grep "$OS" | grep "$ARCH" | cut -d '"' -f 4 | head -n 1)

build_from_source() {
    echo "Checking build dependencies..."
    
    # Ensure C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        echo "C++ compiler not found."
        if [[ "$OS" == "darwin" ]]; then
            echo "Please install Xcode Command Line Tools: xcode-select --install"
            exit 1
        else
            install_pkg g++
        fi
    fi

    # Ensure make
    if ! command -v make &> /dev/null; then
        install_pkg make
    fi

    if [ -f "src/main.cpp" ] && [ -f "Makefile" ]; then
        echo "Local source code detected. Building..."
    else
        echo "Downloading source code..."
        TMP_DIR=$(mktemp -d)
        cd "$TMP_DIR"
        curl -fsSL https://github.com/alonsovm44/gendox/archive/refs/heads/master.tar.gz -o source.tar.gz
        tar -xzf source.tar.gz
        cd gendox-master
    fi

    if [ ! -d "tree-sitter" ]; then
        echo "Downloading tree-sitter dependencies..."
        curl -fsSL https://github.com/tree-sitter/tree-sitter/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-master tree-sitter
        curl -fsSL https://github.com/tree-sitter/tree-sitter-c/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-c-master tree-sitter-c
        curl -fsSL https://github.com/tree-sitter/tree-sitter-cpp/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-cpp-master tree-sitter-cpp
        curl -fsSL https://github.com/tree-sitter/tree-sitter-python/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-python-master tree-sitter-python
        curl -fsSL https://github.com/tree-sitter/tree-sitter-javascript/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-javascript-master tree-sitter-javascript
        curl -fsSL https://github.com/tree-sitter/tree-sitter-typescript/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-typescript-master tree-sitter-typescript
        curl -fsSL https://github.com/tree-sitter/tree-sitter-go/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-go-master tree-sitter-go
        curl -fsSL https://github.com/tree-sitter/tree-sitter-rust/archive/refs/heads/master.tar.gz | tar -xz && mv tree-sitter-rust-master tree-sitter-rust
    fi

    echo "Building gendox from source..."
    make clean
    make all
    mv gendox "$INSTALL_DIR/"
    
    if [ -n "$TMP_DIR" ]; then
        cd "$HOME"
        rm -rf "$TMP_DIR"
    fi
    echo "gendox successfully built and installed to $INSTALL_DIR/gendox"
}

if [[ -n "$DOWNLOAD_URL" ]]; then
    echo "Downloading pre-built binary for $OS-$ARCH..."
    if curl -fsSL -o "$INSTALL_DIR/gendox" "$DOWNLOAD_URL"; then
        chmod +x "$INSTALL_DIR/gendox"
        echo "gendox successfully installed to $INSTALL_DIR/gendox"
    else
        echo "Failed to download pre-built binary. Falling back to source build..."
        build_from_source
    fi
else
    echo "No pre-built binary found for $OS-$ARCH. Falling back to source installation..."
    build_from_source
fi

# Path warning
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "=================================================="
    echo "WARNING: $INSTALL_DIR is not in your PATH."
    echo "Please add it to your shell configuration (e.g., ~/.bashrc or ~/.zshrc):"
    echo "export PATH=\"\$HOME/.local/bin:\$PATH\""
    echo "=================================================="
fi

echo "Setup complete! Run 'gendox' to get started."