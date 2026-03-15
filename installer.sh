#!/bin/bash
set -e

echo "Checking for prerequisites..."
if ! command -v curl &> /dev/null; then
    echo "Error: curl not found. docgen requires curl to make API requests, and this script needs it to download Ollama."
    exit 1
fi

if [ ! -f "src/main.cpp" ]; then
    echo "Error: src/main.cpp not found."
    echo "Please clone the repository and run this script from the project root."
    exit 1
fi

if ! command -v ollama &> /dev/null; then
    install="n"
    if [ -t 0 ]; then
        read -p "Ollama not found. Do you want to install it? (y/n) " install
    elif [ -c /dev/tty ]; then
        echo -n "Ollama not found. Do you want to install it? (y/n) "
        read install < /dev/tty
    fi
    if [[ "$install" == "y" || "$install" == "Y" ]]; then
        echo "Installing Ollama..."
        curl -fsSL https://ollama.com/install.sh | sh
    fi
fi

compiler=""

if command -v g++ &> /dev/null; then
    compiler="g++"
elif command -v clang++ &> /dev/null; then
    compiler="clang++"
else
    echo "No C++ compiler found. Attempting to install g++..."
    
    SUDO=""
    if command -v sudo &> /dev/null; then
        SUDO="sudo"
    fi

    if command -v apt-get &> /dev/null; then
        $SUDO apt-get update && $SUDO apt-get install -y g++
    elif command -v dnf &> /dev/null; then
        $SUDO dnf install -y gcc-c++
    elif command -v yum &> /dev/null; then
        $SUDO yum install -y gcc-c++
    elif command -v pacman &> /dev/null; then
        $SUDO pacman -Sy --noconfirm gcc
    elif command -v zypper &> /dev/null; then
        $SUDO zypper install -y gcc-c++
    elif command -v apk &> /dev/null; then
        $SUDO apk add g++
    elif command -v brew &> /dev/null; then
        brew install gcc
    else
        echo "Error: Could not find a supported package manager. Please install g++ or clang++ manually."
        exit 1
    fi

    if command -v g++ &> /dev/null; then
        compiler="g++"
    else
        echo "Error: Failed to install g++."
        exit 1
    fi
fi

echo "Compiling docgen..."
if [ "$compiler" = "g++" ]; then
    echo "Using g++..."
    g++ -std=c++17 -o docgen src/main.cpp
elif [ "$compiler" = "clang++" ]; then
    echo "Using clang++..."
    clang++ -std=c++17 -o docgen src/main.cpp
fi

if [ -f docgen ]; then
    INSTALL_DIR="$HOME/.local/bin"
    mkdir -p "$INSTALL_DIR"
    mv docgen "$INSTALL_DIR/docgen"
    
    echo "Build complete. 'docgen' was installed to $INSTALL_DIR."
    if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
        echo "Make sure $INSTALL_DIR is in your PATH to run 'docgen' from anywhere."
    else
        echo "Run 'docgen' to start."
    fi
else
    echo "Build failed."
    exit 1
fi