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

    $candidateRoots = @(
        Join-Path $repoRoot "vcpkg_installed\\$Triplet",
        Join-Path $repoRoot "vcpkg_installed\\$Triplet\\$Triplet"
    )
    if ($env:VCPKG_ROOT) {
        $candidateRoots += Join-Path $env:VCPKG_ROOT "installed\\$Triplet"
    }

    $resolvedRoot = $null
    foreach ($root in $candidateRoots) {
        $includeCandidate = Join-Path $root "include"
        $libCandidate = Join-Path $root "lib"
        if ((Test-Path $includeCandidate) -and (Test-Path $libCandidate)) {
            $resolvedRoot = $root
            break
        }
    }
    if (-not $resolvedRoot) {
        $searched = $candidateRoots -join "`n  - "
        throw "Could not locate vcpkg include/lib directories for triplet '$Triplet'. Searched:`n  - $searched"
    }

    $includePath = Join-Path $resolvedRoot "include"
    $libPath = Join-Path $resolvedRoot "lib"

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
