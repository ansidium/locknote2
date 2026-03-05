param(
    [Parameter(Mandatory = $true)]
    [string]$ExePath,
    [Parameter(Mandatory = $true)]
    [string]$Tag,
    [string]$RunnerScript = '',
    [switch]$AllowFallbackToCurrentDesktop
)

$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RunnerScript)) {
    $RunnerScript = Join-Path $PSScriptRoot 'ui-smoke-notepad-redesign.ps1'
}

if (-not (Test-Path -LiteralPath $ExePath)) {
    throw "ExePath not found: $ExePath"
}
if (-not (Test-Path -LiteralPath $RunnerScript)) {
    throw "RunnerScript not found: $RunnerScript"
}

$powershellExe = Join-Path $PSHOME 'powershell.exe'
$resolvedExe = (Resolve-Path -LiteralPath $ExePath).Path
$resolvedRunner = (Resolve-Path -LiteralPath $RunnerScript).Path
$workdir = (Resolve-Path -LiteralPath (Split-Path -Parent $RunnerScript)).Path

$native = @"
using System;
using System.Runtime.InteropServices;

public static class IsolatedDesktopNative {
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct STARTUPINFOW {
        public int cb;
        public string lpReserved;
        public string lpDesktop;
        public string lpTitle;
        public int dwX;
        public int dwY;
        public int dwXSize;
        public int dwYSize;
        public int dwXCountChars;
        public int dwYCountChars;
        public int dwFillAttribute;
        public int dwFlags;
        public short wShowWindow;
        public short cbReserved2;
        public IntPtr lpReserved2;
        public IntPtr hStdInput;
        public IntPtr hStdOutput;
        public IntPtr hStdError;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PROCESS_INFORMATION {
        public IntPtr hProcess;
        public IntPtr hThread;
        public int dwProcessId;
        public int dwThreadId;
    }

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr CreateDesktopW(
        string lpszDesktop,
        IntPtr lpszDevice,
        IntPtr pDevmode,
        int dwFlags,
        uint dwDesiredAccess,
        IntPtr lpsa);

    [DllImport("user32.dll", SetLastError = true)]
    public static extern bool CloseDesktop(IntPtr hDesktop);

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool CreateProcessW(
        string lpApplicationName,
        string lpCommandLine,
        IntPtr lpProcessAttributes,
        IntPtr lpThreadAttributes,
        bool bInheritHandles,
        uint dwCreationFlags,
        IntPtr lpEnvironment,
        string lpCurrentDirectory,
        ref STARTUPINFOW lpStartupInfo,
        out PROCESS_INFORMATION lpProcessInformation);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool GetExitCodeProcess(IntPtr hProcess, out uint lpExitCode);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool CloseHandle(IntPtr hObject);
}
"@

Add-Type -TypeDefinition $native -ReferencedAssemblies 'System.dll'

$desktopName = "CodexUiSmoke_{0}" -f ([Guid]::NewGuid().ToString('N'))
$desktopAccess = 0x000F01FF # Full desktop access for automation and UI interaction.
$desktopHandle = [IsolatedDesktopNative]::CreateDesktopW(
    $desktopName,
    [IntPtr]::Zero,
    [IntPtr]::Zero,
    0,
    [uint32]$desktopAccess,
    [IntPtr]::Zero)

if ($desktopHandle -eq [IntPtr]::Zero) {
    $err = [Runtime.InteropServices.Marshal]::GetLastWin32Error()
    throw "CreateDesktopW failed. Win32Error=$err"
}
Write-Host "Created isolated desktop: $desktopName"

$isolatedSucceeded = $false
$isolatedError = $null
try {
    $escapedExe = $resolvedExe.Replace('"', '""')
    $escapedRunner = $resolvedRunner.Replace('"', '""')
    $escapedTag = $Tag.Replace('"', '""')

    $commandLine = "-NoProfile -ExecutionPolicy Bypass -File `"$escapedRunner`" -ExePath `"$escapedExe`" -Tag `"$escapedTag`""

    $startupInfo = New-Object IsolatedDesktopNative+STARTUPINFOW
    $startupInfo.cb = [Runtime.InteropServices.Marshal]::SizeOf([type]([IsolatedDesktopNative+STARTUPINFOW]))
    $startupInfo.lpDesktop = $desktopName

    $procInfo = New-Object IsolatedDesktopNative+PROCESS_INFORMATION
    $created = [IsolatedDesktopNative]::CreateProcessW(
        $powershellExe,
        $commandLine,
        [IntPtr]::Zero,
        [IntPtr]::Zero,
        $false,
        0,
        [IntPtr]::Zero,
        $workdir,
        [ref]$startupInfo,
        [ref]$procInfo)

    if (-not $created) {
        $err = [Runtime.InteropServices.Marshal]::GetLastWin32Error()
        throw "CreateProcessW failed. Win32Error=$err"
    }
    Write-Host ("Started runner PID={0} on desktop {1}" -f $procInfo.dwProcessId, $desktopName)

    try {
        [void][IsolatedDesktopNative]::WaitForSingleObject($procInfo.hProcess, [uint32]::MaxValue)
        [uint32]$exitCode = 0
        if (-not [IsolatedDesktopNative]::GetExitCodeProcess($procInfo.hProcess, [ref]$exitCode)) {
            $err = [Runtime.InteropServices.Marshal]::GetLastWin32Error()
            throw "GetExitCodeProcess failed. Win32Error=$err"
        }
        Write-Host ("Runner exit code: {0}" -f $exitCode)
        if ($exitCode -ne 0) {
            throw "Smoke runner failed on isolated desktop. ExitCode=$exitCode"
        }
        $isolatedSucceeded = $true
    }
    finally {
        if ($procInfo.hThread -ne [IntPtr]::Zero) {
            [void][IsolatedDesktopNative]::CloseHandle($procInfo.hThread)
        }
        if ($procInfo.hProcess -ne [IntPtr]::Zero) {
            [void][IsolatedDesktopNative]::CloseHandle($procInfo.hProcess)
        }
    }
}
catch {
    $isolatedError = $_
    if (-not $AllowFallbackToCurrentDesktop) {
        throw
    }
    Write-Warning ("Isolated desktop run failed: {0}" -f $_.Exception.Message)
}
finally {
    if ($desktopHandle -ne [IntPtr]::Zero) {
        [void][IsolatedDesktopNative]::CloseDesktop($desktopHandle)
    }
}

if (-not $isolatedSucceeded -and $AllowFallbackToCurrentDesktop) {
    Write-Warning "Falling back to current desktop smoke run."
    & $powershellExe -NoProfile -ExecutionPolicy Bypass -File $resolvedRunner -ExePath $resolvedExe -Tag $Tag
    if ($LASTEXITCODE -ne 0) {
        throw "Fallback smoke runner failed. ExitCode=$LASTEXITCODE"
    }
}

$artifactDir = Join-Path (Split-Path -Parent $PSScriptRoot) 'artifacts\screens'
Write-Output ("before={0}" -f (Join-Path $artifactDir ("locknote2-redesign-before-{0}-final.png" -f $Tag)))
Write-Output ("after={0}" -f (Join-Path $artifactDir ("locknote2-redesign-after-{0}-final.png" -f $Tag)))
Write-Output ("status={0}" -f (Join-Path $artifactDir ("locknote2-statusbar-{0}-final.png" -f $Tag)))
Write-Output ("menu={0}" -f (Join-Path $artifactDir ("locknote2-menu-{0}-final.png" -f $Tag)))
Write-Output ("settings={0}" -f (Join-Path $artifactDir ("locknote2-settings-{0}-final.png" -f $Tag)))
