[CmdletBinding()]
param(
    [switch]$SkipCodeAnalysis,
    [switch]$SkipCppcheck,
    [switch]$SkipSmoke
)

$ErrorActionPreference = "Stop"

function Invoke-Step {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][scriptblock]$Action
    )

    Write-Host "=== $Name ===" -ForegroundColor Cyan
    & $Action
}

$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot
try {
    if (-not (Get-Command msbuild -ErrorAction SilentlyContinue)) {
        throw "msbuild was not found. Run this script from a Visual Studio Developer PowerShell prompt."
    }

    Invoke-Step -Name "Build Debug|Win32" -Action {
        & msbuild ".\\locknote2.sln" "/m" "/t:Restore,Build" "/p:Configuration=Debug;Platform=Win32;VcpkgEnableManifest=true" "/verbosity:minimal"
        if ($LASTEXITCODE -ne 0) { throw "Debug build failed with exit code $LASTEXITCODE" }
    }

    Invoke-Step -Name "Build Release|Win32" -Action {
        & msbuild ".\\locknote2.sln" "/m" "/t:Restore,Build" "/p:Configuration=Release;Platform=Win32;VcpkgEnableManifest=true" "/verbosity:minimal"
        if ($LASTEXITCODE -ne 0) { throw "Release build failed with exit code $LASTEXITCODE" }
    }

    if (-not $SkipCodeAnalysis) {
        Invoke-Step -Name "MSVC Code Analysis (Release|Win32)" -Action {
            & msbuild ".\\locknote.vcxproj" "/m" "/t:Rebuild" "/p:Configuration=Release;Platform=Win32;RunCodeAnalysis=true;VcpkgEnableManifest=true" "/verbosity:minimal"
            if ($LASTEXITCODE -ne 0) { throw "Code analysis failed with exit code $LASTEXITCODE" }
        }
    }

    if (-not $SkipCppcheck -and (Get-Command cppcheck -ErrorAction SilentlyContinue)) {
        Invoke-Step -Name "cppcheck" -Action {
            & cppcheck "--std=c++23" "--language=c++" "--platform=win32W" "--enable=warning,style,performance,portability" "--force" "--inline-suppr" "--suppress=missingIncludeSystem" "--quiet" "."
            if ($LASTEXITCODE -ne 0) { throw "cppcheck reported issues (exit $LASTEXITCODE)" }
        }
    }

    if (-not $SkipSmoke) {
        Invoke-Step -Name "AES smoke tests" -Action {
            & "$PSScriptRoot\\build-and-run-aes-smoke.ps1"
            if ($LASTEXITCODE -ne 0) { throw "AES smoke tests failed with exit code $LASTEXITCODE" }
        }
    }

    Write-Host "QA pipeline completed." -ForegroundColor Green
}
finally {
    Pop-Location
}
