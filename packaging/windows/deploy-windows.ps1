# Windows deployment script for PatternCAD
# Creates a portable deployment with all Qt dependencies

param(
    [string]$BuildDir = "build/Release",
    [string]$DeployDir = "deploy",
    [string]$QtPath = $env:Qt6_DIR
)

Write-Host "PatternCAD Windows Deployment Script" -ForegroundColor Green
Write-Host "======================================" -ForegroundColor Green

# Check if build exists
$ExePath = Join-Path $BuildDir "PatternCAD.exe"
if (-not (Test-Path $ExePath)) {
    Write-Error "PatternCAD.exe not found at: $ExePath"
    Write-Host "Please build the project first: cmake --build build --config Release"
    exit 1
}

# Clean deploy directory
if (Test-Path $DeployDir) {
    Write-Host "Cleaning deploy directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $DeployDir
}

New-Item -ItemType Directory -Force -Path $DeployDir | Out-Null

# Find windeployqt
$WindeployqtPath = ""
if ($QtPath) {
    $WindeployqtPath = Join-Path $QtPath "bin/windeployqt.exe"
}

if (-not $WindeployqtPath -or -not (Test-Path $WindeployqtPath)) {
    # Try to find in PATH
    $WindeployqtPath = (Get-Command windeployqt -ErrorAction SilentlyContinue).Source
}

if (-not $WindeployqtPath) {
    Write-Error "windeployqt not found. Please set Qt6_DIR environment variable or add Qt bin directory to PATH."
    exit 1
}

Write-Host "Using windeployqt: $WindeployqtPath" -ForegroundColor Cyan

# Copy executable
Write-Host "Copying PatternCAD.exe..." -ForegroundColor Cyan
Copy-Item $ExePath $DeployDir

# Run windeployqt
Write-Host "Running windeployqt..." -ForegroundColor Cyan
& $WindeployqtPath --release --no-translations --dir $DeployDir (Join-Path $DeployDir "PatternCAD.exe")

if ($LASTEXITCODE -ne 0) {
    Write-Error "windeployqt failed with exit code: $LASTEXITCODE"
    exit 1
}

# Create portable README
$ReadmeContent = @"
PatternCAD Portable
===================

This is a portable version of PatternCAD that doesn't require installation.

To run PatternCAD:
1. Double-click PatternCAD.exe
2. Your settings will be saved in the same directory

System Requirements:
- Windows 10 or later
- 64-bit processor
- OpenGL support

For more information, visit: https://patterncad.org
"@

Set-Content -Path (Join-Path $DeployDir "README.txt") -Value $ReadmeContent

Write-Host ""
Write-Host "Deployment complete!" -ForegroundColor Green
Write-Host "Portable package created in: $DeployDir" -ForegroundColor Green

# Create ZIP archive
$ZipPath = "PatternCAD-Windows-Portable.zip"
Write-Host ""
Write-Host "Creating ZIP archive: $ZipPath" -ForegroundColor Cyan

if (Test-Path $ZipPath) {
    Remove-Item $ZipPath
}

Compress-Archive -Path "$DeployDir/*" -DestinationPath $ZipPath

Write-Host ""
Write-Host "ZIP archive created: $ZipPath" -ForegroundColor Green
Write-Host "Size: $((Get-Item $ZipPath).Length / 1MB) MB" -ForegroundColor Cyan
