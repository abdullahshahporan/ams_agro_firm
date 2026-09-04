param(
    [switch]$BuildOnly
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$compiler = 'C:\MinGW\bin\g++.exe'
$courseRoot = Join-Path $projectRoot '..\CSE 4208 - Graphics'
$openGlRoot = Join-Path $courseRoot 'Lab_2\opengl'
$lab4CodeRoot = Join-Path $courseRoot 'Lab_4\codes'
$sourceDirectory = Join-Path $projectRoot 'src'
$includeDirectory = Join-Path $projectRoot 'include'
$outputDirectory = Join-Path $projectRoot 'bin\MinGW'
$outputExecutable = Join-Path $outputDirectory 'AMSAgroFarm.exe'
$glfwImportLibraryDirectory = Join-Path $projectRoot 'lib\MinGW'

$requiredPaths = @(
    $compiler,
    (Join-Path $openGlRoot 'include\glad\glad.h'),
    (Join-Path $openGlRoot 'glad.c'),
    (Join-Path $lab4CodeRoot 'stb_image.cpp'),
    (Join-Path $glfwImportLibraryDirectory 'libglfw3dll.a'),
    (Join-Path $outputDirectory 'glfw3.dll')
)

foreach ($requiredPath in $requiredPaths) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required build file is missing: $requiredPath"
    }
}

New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

$projectSources = @(
    Get-ChildItem -LiteralPath $sourceDirectory -Filter '*.cpp' -File |
        Sort-Object Name |
        ForEach-Object FullName
)

if ($projectSources.Count -eq 0) {
    throw "No C++ source files found in: $sourceDirectory"
}

$compilerArguments = @(
    '-std=c++17',
    '-O0',
    '-g',
    '-DGLFW_DLL',
    '-I', $includeDirectory,
    '-I', (Join-Path $openGlRoot 'include'),
    '-I', $lab4CodeRoot
)

$compilerArguments += $projectSources
$compilerArguments += @(
    (Join-Path $openGlRoot 'glad.c'),
    (Join-Path $lab4CodeRoot 'stb_image.cpp'),
    '-L', $glfwImportLibraryDirectory,
    '-lglfw3dll',
    '-lopengl32',
    '-lgdi32',
    '-luser32',
    '-lshell32',
    '-o', $outputExecutable
)

Write-Host 'Building AMS Agro Farm...' -ForegroundColor Cyan
& $compiler $compilerArguments
if ($LASTEXITCODE -ne 0) {
    throw "Compilation failed with exit code $LASTEXITCODE"
}

Write-Host "Build successful: $outputExecutable" -ForegroundColor Green

if (-not $BuildOnly) {
    Write-Host 'Launching Final Module 5...' -ForegroundColor Cyan
    Push-Location $projectRoot
    try {
        & $outputExecutable
    }
    finally {
        Pop-Location
    }
}
