# docgen Self-Upgrade Script Documentation

## Purpose
This script automates the process of upgrading the `docgen` executable to its latest version from a GitHub release. It ensures a seamless update by replacing the current executable with the new version while handling potential conflicts.

## Usage
The script is designed to be executed in a PowerShell environment. It requires administrative privileges if the `docgen` executable is located in a protected directory (e.g., `C:\Program Files`).

### Prerequisites
- PowerShell 5.1 or later.
- Internet connectivity to download the latest release from GitHub.

### Execution
Run the script in a PowerShell session:
```powershell
& { $script = [scriptblock]::Create((Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/alonsovm44/docgen/main/upgrade.ps1').Content); Invoke-Command -ScriptBlock $script }
```

## Behavior
1. **Error Handling**: The script sets `$ErrorActionPreference = 'Stop'` to halt execution on any errors, ensuring failures are immediately reported.
2. **Locate Current Executable**: Identifies the path of the currently installed `docgen` executable using `Get-Command`.
3. **Download Latest Version**: Fetches the latest `docgen.exe` from the specified GitHub release URL.
4. **Backup Current Executable**: Renames the existing `docgen.exe` to `docgen.exe.old` to preserve the previous version, removing any existing `.old` backup beforehand.
5. **Replace with New Version**: Moves the downloaded executable to the original location, effectively upgrading `docgen`.
6. **Post-Upgrade Notification**: Informs the user to restart their terminal for changes to take effect.

### Key Features
- **Atomic Replacement**: Ensures the upgrade is atomic by renaming the old executable and moving the new one into place, avoiding partial updates.
- **Cleanup**: Removes any existing `.old` backup before proceeding to prevent disk space issues.
- **Error Reporting**: Provides clear error messages if the upgrade fails, including the specific issue encountered.

### Edge Cases
- **Network Failure**: If the download fails, the script catches the exception, logs the error, and exits with a non-zero status code.
- **File Access Issues**: If the script cannot rename or replace the executable (e.g., due to permissions), it halts and reports the failure.

## Notes
- Adjust the `$url` variable if the release asset name or location changes.
- The script assumes the `docgen` command is available in the current PATH and is a direct executable file.