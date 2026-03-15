$ErrorActionPreference = "Stop"

Write-Host "Checking for prerequisites..."

if (-not (Get-Command curl -ErrorAction SilentlyContinue)) {
    Write-Warning "curl not found. docgen requires curl to make API requests."
}

if (-not (Test-Path "src/main.cpp")) {
    Write-Error "src/main.cpp not found. Please clone the repository and run this script from the project root."
    exit 1
}

if (-not (Get-Command ollama -ErrorAction SilentlyContinue)) {
    $install = Read-Host "Ollama not found. Do you want to install it? (y/n)"
    if ($install -eq 'y') {
        Write-Host "Downloading OllamaSetup.exe..."
        $installerPath = "$env:TEMP\OllamaSetup.exe"
        Invoke-WebRequest -Uri "https://ollama.com/download/OllamaSetup.exe" -OutFile $installerPath
        Write-Host "Running installer..."
        Start-Process -FilePath $installerPath -Wait
    }
}

Write-Host "Compiling docgen..."

$compiler = $null

if (Get-Command g++ -ErrorAction SilentlyContinue) {
    $compiler = "g++"
} elseif (Get-Command clang++ -ErrorAction SilentlyContinue) {
    $compiler = "clang++"
} elseif (Get-Command cl -ErrorAction SilentlyContinue) {
    $compiler = "cl"
} else {
    Write-Host "No C++ compiler found. Installing MinGW (w64devkit)..."
    
    $zipPath = "$env:TEMP\mingw.zip"
    $installDir = "$env:LOCALAPPDATA\docgen-mingw"
    
    Write-Host "Downloading portable MinGW..."
    Invoke-WebRequest -Uri "https://github.com/skeeto/w64devkit/releases/download/v1.22.0/w64devkit-1.22.0.zip" -OutFile $zipPath
    
    Write-Host "Extracting MinGW to $installDir (this may take a minute)..."
    if (Test-Path $installDir) { Remove-Item -Recurse -Force $installDir }
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [System.IO.Compression.ZipFile]::ExtractToDirectory($zipPath, $installDir)
    
    $mingwPath = "$installDir\w64devkit\bin"
    
    Write-Host "Setting g++ to PATH..."
    $env:PATH += ";$mingwPath"
    
    $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
    if ($userPath -notmatch [regex]::Escape($mingwPath)) {
        [System.Environment]::SetEnvironmentVariable("Path", "$userPath;$mingwPath", "User")
    }
    
    if (Get-Command g++ -ErrorAction SilentlyContinue) {
        $compiler = "g++"
    } else {
        Write-Error "Failed to install or locate g++ after attempting to install MinGW."
        exit 1
    }
}

if ($compiler -eq "g++") {
    Write-Host "Using g++..."
    g++ -std=c++17 -o docgen.exe src/main.cpp
} elseif ($compiler -eq "clang++") {
    Write-Host "Using clang++..."
    clang++ -std=c++17 -o docgen.exe src/main.cpp
} elseif ($compiler -eq "cl") {
    Write-Host "Using MSVC (cl)..."
    # /EHsc: Enable C++ exceptions
    # /std:c++17: Use C++17 standard
    # /Fe:docgen.exe: Output executable name
    cl /EHsc /std:c++17 /Fe:docgen.exe src/main.cpp
}

if (Test-Path docgen.exe) {
    $installDir = "$env:LOCALAPPDATA\docgen\bin"
    if (-not (Test-Path $installDir)) { New-Item -ItemType Directory -Path $installDir | Out-Null }
    Move-Item -Path docgen.exe -Destination "$installDir\docgen.exe" -Force
    
    Write-Host "Adding docgen to PATH..."
    $env:PATH += ";$installDir"
    
    $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
    if ($userPath -notmatch [regex]::Escape($installDir)) {
        [System.Environment]::SetEnvironmentVariable("Path", "$userPath;$installDir", "User")
    }
    Write-Host "Build complete. 'docgen' was installed to $installDir. Run 'docgen' to start."
} else {
    Write-Error "Build failed."
    exit 1
}