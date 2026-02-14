[CmdletBinding()]
param(
    [string]$Triplet = "x86-windows-static"
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot
try {
    if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
        throw "cl.exe was not found. Run this from a Visual Studio Developer PowerShell prompt."
    }

    if (-not $env:VCPKG_ROOT) {
        throw "VCPKG_ROOT is not set. Install dependencies with vcpkg and set VCPKG_ROOT before running this script."
    }

    $includePath = Join-Path $env:VCPKG_ROOT "installed\\$Triplet\\include"
    $libPath = Join-Path $env:VCPKG_ROOT "installed\\$Triplet\\lib"
    if (-not (Test-Path $includePath)) {
        throw "Missing include path: $includePath"
    }
    if (-not (Test-Path $libPath)) {
        throw "Missing library path: $libPath"
    }

    $outExe = Join-Path $repoRoot "tests\\aeslayer_smoke.exe"
    if (Test-Path $outExe) {
        Remove-Item $outExe -Force
    }

    $compileArgs = @(
        "/nologo",
        "/std:c++23preview",
        "/EHsc",
        "/W4",
        "/I.",
        "/I$includePath",
        "tests\\aeslayer_smoke.cpp",
        "aeslayer.cpp",
        "/link",
        "/LIBPATH:$libPath",
        "cryptlib.lib",
        "/OUT:$outExe"
    )

    & cl.exe @compileArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Smoke test compilation failed with exit code $LASTEXITCODE"
    }

    & $outExe
    if ($LASTEXITCODE -ne 0) {
        throw "Smoke test execution failed with exit code $LASTEXITCODE"
    }
}
finally {
    Pop-Location
}
