$ErrorActionPreference='Stop'
$taskRoot=Split-Path -Parent $PSScriptRoot
Push-Location $taskRoot
try {
    $taskSources=@(Get-ChildItem src -Recurse -File -Filter '*.cpp' | Where-Object { $_.Name -notin @('main.cpp','simulation.cpp') } | Sort-Object FullName | ForEach-Object FullName)
    New-Item -ItemType Directory -Force tmp | Out-Null
    $taskArgs=@('-std=c++17','-O2','-DGLFW_DLL','-I','include','-I','..\CSE 4208 - Graphics\Lab_2\opengl\include','-I','..\CSE 4208 - Graphics\Lab_4\codes','tests\interaction_regression.cpp')+$taskSources
    $taskArgs+=@('..\CSE 4208 - Graphics\Lab_2\opengl\glad.c','..\CSE 4208 - Graphics\Lab_4\codes\stb_image.cpp','-L','lib\MinGW','-lglfw3dll','-lopengl32','-lgdi32','-o','tmp\interaction_regression.exe')
    & 'C:\MinGW\bin\g++.exe' $taskArgs
    if($LASTEXITCODE -ne 0){throw 'Regression test build failed'}
    $env:PATH=(Join-Path $taskRoot 'bin\MinGW')+';'+$env:PATH
    & .\tmp\interaction_regression.exe
    if($LASTEXITCODE -ne 0){throw 'Interaction regression failed'}
}finally {Pop-Location}
