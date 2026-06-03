param(
  [string]$OpenCpnPath = "..\external\OpenCPN",
  [switch]$Force
)

$src = Resolve-Path $OpenCpnPath
if (-not (Test-Path $src)) {
  Write-Error "OpenCPN path not found: $OpenCpnPath"
  exit 1
}

$files = @(
  "gui/src/cm93.cpp",
  "gui/include/gui/cm93.h"
)

$dest = Join-Path $PSScriptRoot "external"
New-Item -ItemType Directory -Path $dest -Force | Out-Null

foreach ($f in $files) {
  $sourceFile = Join-Path $src $f
  if (-not (Test-Path $sourceFile)) {
    Write-Warning "Source file missing: $sourceFile"
    continue
  }
  $destFile = Join-Path $dest ([IO.Path]::GetFileName($f))
  if ((-not (Test-Path $destFile)) -or $Force) {
    Copy-Item -Path $sourceFile -Destination $destFile -Force
    Write-Host "Copied $f -> $destFile"
  } else {
    Write-Host "Exists: $destFile (use -Force to overwrite)"
  }
}

Write-Host "Import complete. Edit stubs in include/ocpn_stubs.h as needed."
