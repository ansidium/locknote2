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
        (Join-Path -Path $repoRoot -ChildPath "vcpkg_installed\\$Triplet")
        (Join-Path -Path $repoRoot -ChildPath "vcpkg_installed\\$Triplet\\$Triplet")
    )
    if ($env:VCPKG_ROOT) {
        $candidateRoots += (Join-Path -Path $env:VCPKG_ROOT -ChildPath "installed\\$Triplet")
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
    $debugLibPath = Join-Path $resolvedRoot "debug\\lib"
    $librarySearchPaths = @($libPath)
    if ((Test-Path $debugLibPath) -and ($debugLibPath -ne $libPath)) {
        $librarySearchPaths += $debugLibPath
    }

    $cryptoLibName = $null
    foreach ($searchPath in $librarySearchPaths) {
        foreach ($candidate in @("cryptopp.lib", "cryptoppd.lib", "cryptlib.lib")) {
            if (Test-Path (Join-Path $searchPath $candidate)) {
                $cryptoLibName = $candidate
                break
            }
        }
        if ($cryptoLibName) {
            break
        }
    }
    if (-not $cryptoLibName) {
        $searchedPaths = $librarySearchPaths -join "`n  - "
        throw "Could not find Crypto++ library file in:`n  - $searchedPaths"
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
        $cryptoLibName,
        "/OUT:$outExe"
    )
    if ((Test-Path $debugLibPath) -and ($debugLibPath -ne $libPath)) {
        $compileArgs += "/LIBPATH:$debugLibPath"
    }

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
