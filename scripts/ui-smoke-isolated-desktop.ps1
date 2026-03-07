[CmdletBinding()]
param(
    [string]$Win32ExePath = "",
    [string]$X64ExePath = "",
    [int[]]$DpiScales = @(100, 125, 150),
    [switch]$SkipBuild,
    [string]$OutputDir = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$runnerScript = Join-Path $PSScriptRoot "ui-smoke-notepad-redesign.ps1"
$powershellExe = Join-Path $PSHOME "powershell.exe"

if (-not (Test-Path -LiteralPath $runnerScript)) {
    throw "Runner script was not found: $runnerScript"
}

$resolvedOutputDir = if ([string]::IsNullOrWhiteSpace($OutputDir)) {
    Join-Path $repoRoot "artifacts\\screens\\ui-smoke"
}
else {
    if ([System.IO.Path]::IsPathRooted($OutputDir)) {
        $OutputDir
    }
    else {
        Join-Path $repoRoot $OutputDir
    }
}
if (-not (Test-Path -LiteralPath $resolvedOutputDir)) {
    New-Item -Path $resolvedOutputDir -ItemType Directory -Force | Out-Null
}

$resolvedWin32ExePath = if ([string]::IsNullOrWhiteSpace($Win32ExePath)) {
    Join-Path $repoRoot "Release\\LockNote2.exe"
}
elseif ([System.IO.Path]::IsPathRooted($Win32ExePath)) {
    $Win32ExePath
}
else {
    Join-Path $repoRoot $Win32ExePath
}

$resolvedX64ExePath = if ([string]::IsNullOrWhiteSpace($X64ExePath)) {
    Join-Path $repoRoot "x64\\Release\\LockNote2.exe"
}
elseif ([System.IO.Path]::IsPathRooted($X64ExePath)) {
    $X64ExePath
}
else {
    Join-Path $repoRoot $X64ExePath
}

if (-not $SkipBuild) {
    if (-not (Get-Command msbuild -ErrorAction SilentlyContinue)) {
        throw "msbuild was not found. Run this script from a Visual Studio Developer PowerShell prompt."
    }

    Write-Host "=== Build Release|Win32 ===" -ForegroundColor Cyan
    & msbuild ".\\locknote2.sln" "/m" "/t:Build" "/p:Configuration=Release;Platform=Win32;VcpkgEnableManifest=true" "/verbosity:minimal"
    if ($LASTEXITCODE -ne 0) {
        throw "Win32 build failed with exit code $LASTEXITCODE"
    }

    Write-Host "=== Build Release|x64 ===" -ForegroundColor Cyan
    & msbuild ".\\locknote2.sln" "/m" "/t:Build" "/p:Configuration=Release;Platform=x64;VcpkgEnableManifest=true" "/verbosity:minimal"
    if ($LASTEXITCODE -ne 0) {
        throw "x64 build failed with exit code $LASTEXITCODE"
    }
}

$targets = @(
    [pscustomobject]@{
        Tag = "win32"
        ExePath = $resolvedWin32ExePath
    },
    [pscustomobject]@{
        Tag = "x64"
        ExePath = $resolvedX64ExePath
    }
)

foreach ($target in $targets) {
    if (-not (Test-Path -LiteralPath $target.ExePath)) {
        throw "Executable for target '$($target.Tag)' was not found: $($target.ExePath)"
    }
}

$results = New-Object System.Collections.Generic.List[object]

foreach ($target in $targets) {
    foreach ($dpiScale in $DpiScales) {
        Write-Host ("=== UI smoke {0} @ {1}% DPI ===" -f $target.Tag, $dpiScale) -ForegroundColor Cyan
        $output = & $powershellExe -NoProfile -ExecutionPolicy Bypass -File $runnerScript `
            -ExePath $target.ExePath `
            -Tag $target.Tag `
            -DpiScale $dpiScale `
            -OutputDir $resolvedOutputDir 2>&1
        $exitCode = $LASTEXITCODE

        $outputLines = @()
        foreach ($line in $output) {
            if ($line -ne $null) {
                $outputLines += [string]$line
            }
        }

        $jsonLine = $outputLines | Where-Object { $_.TrimStart().StartsWith("{") -and $_.TrimEnd().EndsWith("}") } | Select-Object -Last 1
        if ([string]::IsNullOrWhiteSpace($jsonLine)) {
            throw ("Runner produced no JSON result for target={0}, dpi={1}. Output:`n{2}" -f
                $target.Tag, $dpiScale, ($outputLines -join [Environment]::NewLine))
        }

        $result = $jsonLine | ConvertFrom-Json
        $results.Add($result)

        if ($exitCode -ne 0) {
            throw ("Runner failed for target={0}, dpi={1}. Output:`n{2}" -f
                $target.Tag, $dpiScale, ($outputLines -join [Environment]::NewLine))
        }
    }
}

$summaryPath = Join-Path $resolvedOutputDir "ui-smoke-matrix-summary.json"
$results | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $summaryPath -Encoding UTF8

$results |
    Sort-Object tag, dpi |
    Select-Object tag, dpi, passed, diff_short_overflow, diff_short_back |
    Format-Table -AutoSize

Write-Output ("summary={0}" -f $summaryPath)
