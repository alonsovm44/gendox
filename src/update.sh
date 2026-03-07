#!/bin/bash
set -e

echo "Upgrading docgen..."

# Detect OS
OS="$(uname -s)"
if [ "$OS" = "Linux" ]; then
    ASSET="docgen-linux"
elif [ "$OS" = "Darwin" ]; then
    ASSET="docgen-macos"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

URL="https://github.com/alonsovm44/docgen/releases/latest/download/$ASSET"

# Download
echo "Downloading latest version..."
curl -fsSL "$URL" -o /tmp/docgen_new
chmod +x /tmp/docgen_new

# Locate and replace
TARGET=$(which docgen)
if [ -z "$TARGET" ]; then
    echo "Error: docgen not found in PATH."
    exit 1
fi

echo "Replacing $TARGET..."
mv /tmp/docgen_new "$TARGET" || sudo mv /tmp/docgen_new "$TARGET"

echo "Upgrade complete."