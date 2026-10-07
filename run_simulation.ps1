param([switch]$BuildOnly)
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'run_project.ps1') -Simulation -BuildOnly:$BuildOnly
