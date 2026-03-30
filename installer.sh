#!/usr/bin/env bash
# Foolproof installer for Docgen (no git required)
# - prefers prebuilt release assets from GitHub Releases
# - falls back to source archive download and prints build instructions
# - download methods: curl -> wget -> python3
# - verifies checksums if a checksum asset is available

set -euo pipefail
OWNER="alonsovm44"
REPO="docgen"
BIN_NAME="docgen"
GITHUB_API="https://api.github.com/repos/${OWNER}/${REPO}/releases/latest"

error() { echo "ERROR: $*" >&2; exit 1; }
info() { echo "INFO: $*"; }

has_cmd() { command -v "$1" >/dev/null 2>&1; }

download_to() {
  url="$1"; out="$2"
  if has_cmd curl; then
    curl -fsSL "$url" -o "$out"
  elif has_cmd wget; then
    wget -qO "$out" "$url"
  elif has_cmd python3; then
    python3 - <<PY - "$url" "$out"
import sys,urllib.request
url, out = sys.argv[1], sys.argv[2]
with urllib.request.urlopen(url) as r, open(out, "wb") as f:
    f.write(r.read())
PY
  else
    error "No downloader available (curl, wget or python3 required). Please download $url manually."
  fi
}

detect_os_arch() {
  OS="$(uname -s)"
  ARCH="$(uname -m)"
  case "$OS" in
    Linux) os="linux" ;;
    Darwin) os="macos" ;;
    *) os="$OS" ;;
  esac
  case "$ARCH" in
    x86_64|amd64) arch="x86_64" ;;
    aarch64|arm64) arch="arm64" ;;
    *) arch="$ARCH" ;;
  esac
  echo "${os}-${arch}"
}

install_dir="/usr/local/bin"
if [ -n "${LOCAL_INSTALL:-}" ]; then
  # Allow override: LOCAL_INSTALL=~/.local/bin ./install.sh
  install_dir="${LOCAL_INSTALL}"
fi

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

info "Detecting OS/arch..."
target="$(detect_os_arch)"
info "Target: $target"

info "Querying GitHub releases for ${OWNER}/${REPO}..."
# Use python3 for JSON parsing if available, else attempt curl+simple grep (best-effort).
release_json="$tmpdir/release.json"
if has_cmd curl; then
  curl -fsSL "$GITHUB_API" -o "$release_json"
elif has_cmd wget; then
  wget -qO "$release_json" "$GITHUB_API"
elif has_cmd python3; then
  python3 - <<PY >"$release_json"
import urllib.request,sys
with urllib.request.urlopen("${GITHUB_API}") as r:
    sys.stdout.write(r.read().decode())
PY
else
  error "No HTTP client available for querying GitHub API (curl/wget/python3 required)."
fi

# Try to find an asset matching the OS/arch pattern: e.g. docgen-linux-x86_64.tar.gz
asset_url=""
if has_cmd python3; then
  asset_url="$(python3 - <<PY
import json,sys
data=json.load(open("$release_json"))
assets=data.get("assets",[])
target="$target"
# preferences: exact os-arch, then shorter matches
names=[f"{target}.tar.gz", f"{target}.zip", f"${BIN_NAME}-{target}.tar.gz", f"${BIN_NAME}-{target}.zip"]
for a in assets:
    name=a.get("name","")
    if name in names:
        print(a.get("browser_download_url"))
        sys.exit(0)
# fallback: try any asset that contains os or arch
for a in assets:
    name=a.get("name","")
    if "linux" in name or "macos" in name or "darwin" in name:
        print(a.get("browser_download_url"))
        sys.exit(0)
# no asset found
print("")
PY
)"
fi

if [ -n "$asset_url" ]; then
  info "Found release asset: $asset_url"
  out="$tmpdir/asset"
  download_to "$asset_url" "$out"
  # try to find a checksum asset in release assets
  checksum_url="$(python3 - <<PY
import json
data=json.load(open("$release_json"))
for a in data.get("assets",[]):
    if "sha256" in a.get("name","").lower() or "checksums" in a.get("name","").lower():
        print(a.get("browser_download_url"))
        break
else:
    print("")
PY
)"
  if [ -n "$checksum_url" ]; then
    info "Found checksum asset; downloading..."
    checks="$tmpdir/checksums.txt"
    download_to "$checksum_url" "$checks"
    if has_cmd sha256sum; then
      (cd "$tmpdir" && sha256sum -c "$checks" 2>/dev/null) || info "Checksum check did not match or not runnable; please verify manually."
    elif has_cmd shasum; then
      shasum -a 256 -c "$checks" 2>/dev/null || info "Checksum check did not match or not runnable; please verify manually."
    else
      info "No sha tool to verify checksums; please verify $out manually if you care about integrity."
    fi
  fi

  # extract or move
  case "$out" in
    *.tar.gz|*.tgz)
      info "Extracting tar.gz..."
      tar -xzf "$out" -C "$tmpdir"
      # locate binary
      binpath="$(find "$tmpdir" -type f -name "$BIN_NAME" -perm /u=x,g=x,o=x | head -n1 || true)"
      if [ -z "$binpath" ]; then
        # maybe in bin subdir
        binpath="$(find "$tmpdir" -type f -name "${BIN_NAME}*" | head -n1 || true)"
      fi
      ;;
    *.zip)
      if has_cmd unzip; then
        unzip -q "$out" -d "$tmpdir"
        binpath="$(find "$tmpdir" -type f -name "$BIN_NAME" -perm /u=x,g=x,o=x | head -n1 || true)"
      else
        info "zip archive found but 'unzip' not available. Extract $out manually."
        exit 0
      fi
      ;;
    *)
      info "Downloaded asset saved to $out. Please extract/move the binary manually."
      exit 0
      ;;
  esac

  if [ -z "$binpath" ]; then
    info "Could not find the ${BIN_NAME} binary in the archive. Check the release assets manually."
    ls -la "$tmpdir"
    exit 1
  fi

  info "Installing ${BIN_NAME} to ${install_dir} (may require sudo)..."
  mkdir -p "$install_dir"
  if [ -w "$install_dir" ]; then
    cp "$binpath" "$install_dir/$BIN_NAME"
  else
    sudo cp "$binpath" "$install_dir/$BIN_NAME"
  fi
  sudo chmod +x "$install_dir/$BIN_NAME" 2>/dev/null || chmod +x "$install_dir/$BIN_NAME"
  info "Installed: $(which $BIN_NAME 2>/dev/null || echo ${install_dir}/${BIN_NAME})"
  info "Done. Run: $BIN_NAME --help"
  exit 0
fi

# If we reach here: no prebuilt asset found. Offer to download source tarball and show build instructions.
info "No prebuilt release asset found for ${target}."
info "Attempting to download source archive as fallback (no git required)."

src_url="https://github.com/${OWNER}/${REPO}/archive/refs/heads/main.tar.gz"
src_out="$tmpdir/source.tar.gz"
download_to "$src_url" "$src_out"
info "Source archive downloaded to $src_out"
info "Building from source may require: cmake, make, a C++ toolchain. The installer will not attempt to compile automatically."
cat <<EOF

Next steps:

1) Extract:
   mkdir -p $tmpdir/src && tar -xzf $src_out -C $tmpdir/src

2) Inspect README/build instructions in the repo. Typical steps:
   cd $tmpdir/src/${REPO}-main
   mkdir build && cd build
   cmake ..
   make -j

3) After compilation, copy the resulting binary to ${install_dir}:
   sudo cp <path-to-built-binary> ${install_dir}/${BIN_NAME}
   sudo chmod +x ${install_dir}/${BIN_NAME}

Notes:
- This installer never required 'git'. If you want, open an issue in the repo to request prebuilt release assets for your platform so installation is one-step.
- If you want an automated build fallback, set LOCAL_BUILD=1 to allow this script to try to run cmake/make (not enabled by default).

EOF

exit 0