$ErrorActionPreference = 'Stop'

# URL to the new executable (adjust if your release asset name differs)
$url = "https://github.com/alonsovm44/docgen/releases/latest/download/docgen.exe"

try {
    # Get current executable path
    $currentPath = (Get-Command docgen).Source
    Write-Host "Found docgen at: $currentPath"

    Write-Host "Downloading latest version..."
    $tempPath = "$env:TEMP\docgen_new.exe"
    Invoke-WebRequest -Uri $url -OutFile $tempPath

    # On Windows, we can rename a running executable but not overwrite it.
    # We rename the current one to .old and move the new one in place.
    $oldPath = "$currentPath.old"
    if (Test-Path $oldPath) { Remove-Item $oldPath -Force }
    Move-Item -Path $currentPath -Destination $oldPath -Force
    Move-Item -Path $tempPath -Destination $currentPath -Force

    Write-Host "Upgrade successful! Please restart your terminal."
} catch {
    Write-Error "Upgrade failed: $_"
    exit 1
} 