<#
Foolproof PowerShell installer for Docgen (no git required)
- prefers prebuilt release assets from GitHub Releases
- falls back to source archive download and prints build instructions
- uses Invoke-WebRequest / PowerShell webclient behaviour
#>

param(
  [string]$Owner = "alonsovm44",
  [string]$Repo  = "docgen",
  [string]$InstallDir = "$env:USERPROFILE\\bin"  # user-local default
)

function Write-Info { param($m) Write-Host "[INFO] $m" }
function Write-ErrorExit { param($m) Write-Host "[ERROR] $m"; exit 1 }

# Ensure install dir exists and is on PATH
if (-not (Test-Path -Path $InstallDir)) {
  New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
}
if (-not ($Env:PATH -split ";" | ForEach-Object { $_.Trim() } | Where-Object { $_ -eq $InstallDir })) {
  Write-Info "Note: $InstallDir is not on PATH for current session. You can add it to PATH manually or re-open terminal after install."
}

$api = "https://api.github.com/repos/$Owner/$Repo/releases/latest"
$tmp = New-Item -ItemType Directory -Force -Path ([IO.Path]::Combine($env:TEMP, "$Repo-install-" + [Guid]::NewGuid()))

Write-Info "Querying GitHub releases..."
try {
  $resp = Invoke-RestMethod -Uri $api -UseBasicParsing -ErrorAction Stop
} catch {
  Write-ErrorExit "Failed to query GitHub API: $_"
}

# Prefer asset matching OS/arch name
$os = if ($IsWindows) { "windows" } else { "unknown" }
$arch = if ([Environment]::Is64BitOperatingSystem) { "x86_64" } else { "x86" }
$target_names = @("${os}-${arch}.zip","${Repo}-${os}-${arch}.zip","${os}-${arch}.zip")
$asset = $null
foreach ($n in $target_names) {
  $asset = $resp.assets | Where-Object { $_.name -eq $n }
  if ($asset) { break }
}
if (-not $asset) {
  # fallback: first windows asset
  $asset = $resp.assets | Where-Object { $_.name -match "windows" } | Select-Object -First 1
}

if ($asset) {
  $url = $asset.browser_download_url
  Write-Info "Found release asset: $($asset.name). Downloading..."
  $out = Join-Path $tmp $asset.name
  try {
    Invoke-WebRequest -Uri $url -OutFile $out -UseBasicParsing -ErrorAction Stop
  } catch {
    Write-ErrorExit "Failed to download asset: $_"
  }

  # If it's a zip, extract
  if ($out -like "*.zip") {
    Write-Info "Extracting $out..."
    Expand-Archive -Path $out -DestinationPath $tmp -Force
    # look for executable
    $exe = Get-ChildItem -Path $tmp -Recurse -File | Where-Object { $_.Name -match "\.exe$" } | Select-Object -First 1
    if (-not $exe) {
      Write-ErrorExit "No executable found in archive. Contents: $(Get-ChildItem -Path $tmp -Recurse -File | Select-Object -First 20 | ForEach-Object { $_.FullName } )"
    }
    $dest = Join-Path $InstallDir $exe.Name
    Copy-Item -Path $exe.FullName -Destination $dest -Force
    Write-Info "Installed $($exe.Name) to $InstallDir"
    Write-Info "If $InstallDir is not on PATH, add it or move the binary to a folder that is."
    exit 0
  } else {
    Write-Info "Downloaded asset to $out. Please extract and move the binary to $InstallDir."
    exit 0
  }
}

# fallback: download source tarball
$src = "https://github.com/${Owner}/${Repo}/archive/refs/heads/main.zip"
$outsrc = Join-Path $tmp "source.zip"
Write-Info "No prebuilt release asset found for your platform. Downloading source archive as a fallback..."
try {
  Invoke-WebRequest -Uri $src -OutFile $outsrc -UseBasicParsing -ErrorAction Stop
} catch {
  Write-ErrorExit "Failed to download source archive: $_"
}
Write-Host ""
Write-Host "Source archive downloaded to: $outsrc"
Write-Host "Building from source on Windows typically requires visual studio / msbuild / cmake. The installer will not attempt to build automatically."
Write-Host ""
Write-Host "Suggested steps:"
Write-Host "  1) Extract the zip: Expand-Archive -Path $outsrc -DestinationPath $tmp"
Write-Host "  2) Open the project in Visual Studio or follow the repo README build instructions."
Write-Host "  3) After building, copy the resulting binary to $InstallDir"
Write-Host ""
Write-Host "Tip: request prebuilt Windows release assets in the repo to enable one-step installs."
exit 0