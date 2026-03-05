param(
    [Parameter(Mandatory = $true)]
    [string]$ExePath,
    [Parameter(Mandatory = $true)]
    [string]$Tag
)

$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms

$cs = @"
using System;
using System.Runtime.InteropServices;

public static class NativeUi {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }

    [StructLayout(LayoutKind.Sequential)]
    public struct POINT { public int X; public int Y; }

    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr hWndParent, EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern IntPtr FindWindowEx(IntPtr hWndParent, IntPtr hWndChildAfter, string lpszClass, string lpszWindow);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern int GetClassNameW(IntPtr hWnd, System.Text.StringBuilder lpClassName, int nMaxCount);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT lpPoint);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint dwData, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern byte VkKeyScanW(char ch);
    [DllImport("user32.dll")] public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr hWnd, IntPtr hdcBlt, uint nFlags);

    public const uint MOUSEEVENTF_LEFTDOWN = 0x0002;
    public const uint MOUSEEVENTF_LEFTUP = 0x0004;
    public const uint KEYEVENTF_KEYUP = 0x0002;
    public const byte VK_MENU = 0x12;
    public const byte VK_SNAPSHOT = 0x2C;

    public const int SW_RESTORE = 9;
    public const int SW_MAXIMIZE = 3;

    public const uint SWP_NOZORDER = 0x0004;
    public const uint SWP_NOACTIVATE = 0x0010;
}
"@

Add-Type -TypeDefinition $cs -ReferencedAssemblies 'System.dll'

function Get-MainWindowHandle {
    param([System.Diagnostics.Process]$Process)

    $Process.Refresh()
    if ($Process.MainWindowHandle -ne 0) {
        return [IntPtr]$Process.MainWindowHandle
    }

    $result = [IntPtr]::Zero
    [NativeUi]::EnumWindows({
        param($hWnd, $lParam)
        [uint32]$windowPid = 0
        [NativeUi]::GetWindowThreadProcessId($hWnd, [ref]$windowPid) | Out-Null
        if ($windowPid -eq $Process.Id -and [NativeUi]::IsWindowVisible($hWnd)) {
            [NativeUi+RECT]$r = New-Object NativeUi+RECT
            [NativeUi]::GetWindowRect($hWnd, [ref]$r) | Out-Null
            if (($r.Right - $r.Left) -gt 100 -and ($r.Bottom - $r.Top) -gt 100) {
                $script:result = $hWnd
                return $false
            }
        }
        return $true
    }, [IntPtr]::Zero) | Out-Null

    return $result
}

function Get-ChildByClass {
    param([IntPtr]$Parent, [string]$ClassName)

    return [NativeUi]::FindWindowEx($Parent, [IntPtr]::Zero, $ClassName, $null)
}

function Save-WindowShot {
    param([IntPtr]$Hwnd, [string]$Path)

    [NativeUi+RECT]$r = New-Object NativeUi+RECT
    [NativeUi]::GetWindowRect($Hwnd, [ref]$r) | Out-Null
    $w = $r.Right - $r.Left
    $h = $r.Bottom - $r.Top

    $bmp = New-Object System.Drawing.Bitmap($w, $h, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $gfx = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $gfx.GetHdc()
    [NativeUi]::PrintWindow($Hwnd, $hdc, 2) | Out-Null
    $gfx.ReleaseHdc($hdc)
    $gfx.Dispose()
    $bmp.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}

function Invoke-AltPrintScreen {
    [NativeUi]::keybd_event([NativeUi]::VK_MENU, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 25
    [NativeUi]::keybd_event([NativeUi]::VK_SNAPSHOT, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 20
    [NativeUi]::keybd_event([NativeUi]::VK_SNAPSHOT, 0, [NativeUi]::KEYEVENTF_KEYUP, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 20
    [NativeUi]::keybd_event([NativeUi]::VK_MENU, 0, [NativeUi]::KEYEVENTF_KEYUP, [UIntPtr]::Zero)
}

function Save-ActiveWindowShot {
    param([IntPtr]$Hwnd, [string]$Path)

    [NativeUi]::SetForegroundWindow($Hwnd) | Out-Null
    Start-Sleep -Milliseconds 160
    [System.Windows.Forms.Clipboard]::Clear()
    Invoke-AltPrintScreen

    $captured = $false
    for ($retry = 0; $retry -lt 20; $retry++) {
        Start-Sleep -Milliseconds 60
        if ([System.Windows.Forms.Clipboard]::ContainsImage()) {
            $img = [System.Windows.Forms.Clipboard]::GetImage()
            if ($img -ne $null) {
                $img.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
                $img.Dispose()
                $captured = $true
                break
            }
        }
    }

    if (-not $captured) {
        Save-WindowShot -Hwnd $Hwnd -Path $Path
    }
}

function Save-ChildShot {
    param([IntPtr]$Hwnd, [string]$Path)

    [NativeUi+RECT]$r = New-Object NativeUi+RECT
    [NativeUi]::GetWindowRect($Hwnd, [ref]$r) | Out-Null
    $w = $r.Right - $r.Left
    $h = $r.Bottom - $r.Top

    $bmp = New-Object System.Drawing.Bitmap($w, $h, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $gfx = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $gfx.GetHdc()
    [NativeUi]::PrintWindow($Hwnd, $hdc, 2) | Out-Null
    $gfx.ReleaseHdc($hdc)
    $gfx.Dispose()
    $bmp.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}

function Invoke-Click {
    param([int]$X, [int]$Y)

    [NativeUi]::SetCursorPos($X, $Y) | Out-Null
    Start-Sleep -Milliseconds 30
    [NativeUi]::mouse_event([NativeUi]::MOUSEEVENTF_LEFTDOWN, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 20
    [NativeUi]::mouse_event([NativeUi]::MOUSEEVENTF_LEFTUP, 0, 0, 0, [UIntPtr]::Zero)
}

function Invoke-PressEscape {
    [byte]$vk = 0x1B
    [NativeUi]::keybd_event($vk, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 20
    [NativeUi]::keybd_event($vk, 0, [NativeUi]::KEYEVENTF_KEYUP, [UIntPtr]::Zero)
}

$artifactDir = Join-Path (Split-Path -Parent $PSScriptRoot) 'artifacts\screens'
if (-not (Test-Path $artifactDir)) {
    New-Item -Path $artifactDir -ItemType Directory | Out-Null
}

$beforePath = Join-Path $artifactDir ("locknote2-redesign-before-{0}-final.png" -f $Tag)
$afterPath = Join-Path $artifactDir ("locknote2-redesign-after-{0}-final.png" -f $Tag)
$statusPath = Join-Path $artifactDir ("locknote2-statusbar-{0}-final.png" -f $Tag)

$proc = Start-Process -FilePath $ExePath -PassThru
Start-Sleep -Milliseconds 1300

$mainHwnd = [IntPtr]::Zero
for ($i = 0; $i -lt 40; $i++) {
    $mainHwnd = Get-MainWindowHandle -Process $proc
    if ($mainHwnd -ne [IntPtr]::Zero) { break }
    Start-Sleep -Milliseconds 100
}
if ($mainHwnd -eq [IntPtr]::Zero) {
    throw "Main window not found for process $($proc.Id)"
}

[NativeUi]::ShowWindow($mainHwnd, [NativeUi]::SW_RESTORE) | Out-Null
[NativeUi]::SetForegroundWindow($mainHwnd) | Out-Null
Start-Sleep -Milliseconds 300

Save-ActiveWindowShot -Hwnd $mainHwnd -Path $beforePath

[NativeUi+RECT]$wc = New-Object NativeUi+RECT
[NativeUi]::GetClientRect($mainHwnd, [ref]$wc) | Out-Null

$menuY = 52
$menuXs = @(42, 95, 205, 320, 430)
for ($round = 0; $round -lt 5; $round++) {
    foreach ($mx in $menuXs) {
        [NativeUi+POINT]$pt = New-Object NativeUi+POINT
        $pt.X = $mx
        $pt.Y = $menuY
        [NativeUi]::ClientToScreen($mainHwnd, [ref]$pt) | Out-Null
        Invoke-Click -X $pt.X -Y $pt.Y
        Start-Sleep -Milliseconds 80
        Invoke-PressEscape
        Start-Sleep -Milliseconds 40
    }
}

$hoverPoints = @(
    @(32, 50), @(150, 50), @(280, 50), @(420, 50), @(560, 50),
    @([Math]::Max(700, $wc.Right - 360), 50), @([Math]::Max(760, $wc.Right - 280), 50),
    @([Math]::Max(840, $wc.Right - 210), 50), @([Math]::Max(930, $wc.Right - 140), 50)
)
foreach ($pair in $hoverPoints) {
    [NativeUi+POINT]$pt = New-Object NativeUi+POINT
    $pt.X = [int]$pair[0]
    $pt.Y = [int]$pair[1]
    [NativeUi]::ClientToScreen($mainHwnd, [ref]$pt) | Out-Null
    [NativeUi]::SetCursorPos($pt.X, $pt.Y) | Out-Null
    Start-Sleep -Milliseconds 55
}

[NativeUi+RECT]$wr = New-Object NativeUi+RECT
[NativeUi]::GetWindowRect($mainHwnd, [ref]$wr) | Out-Null
$origW = $wr.Right - $wr.Left
$origH = $wr.Bottom - $wr.Top
$newW = [Math]::Max(900, $origW - 280)
$newH = [Math]::Max(620, $origH - 180)
[NativeUi]::SetWindowPos($mainHwnd, [IntPtr]::Zero, $wr.Left + 20, $wr.Top + 20, $newW, $newH, [NativeUi]::SWP_NOZORDER) | Out-Null
Start-Sleep -Milliseconds 220
[NativeUi]::ShowWindow($mainHwnd, [NativeUi]::SW_MAXIMIZE) | Out-Null
Start-Sleep -Milliseconds 220
[NativeUi]::ShowWindow($mainHwnd, [NativeUi]::SW_RESTORE) | Out-Null
[NativeUi]::SetWindowPos($mainHwnd, [IntPtr]::Zero, $wr.Left, $wr.Top, $origW, $origH, [NativeUi]::SWP_NOZORDER) | Out-Null
Start-Sleep -Milliseconds 260

# dropdown check on View menu
[NativeUi+POINT]$viewPt = New-Object NativeUi+POINT
$viewPt.X = 250
$viewPt.Y = $menuY
[NativeUi]::ClientToScreen($mainHwnd, [ref]$viewPt) | Out-Null
Invoke-Click -X $viewPt.X -Y $viewPt.Y
Start-Sleep -Milliseconds 120
[NativeUi+POINT]$dropPt = New-Object NativeUi+POINT
$dropPt.X = 290
$dropPt.Y = 140
[NativeUi]::ClientToScreen($mainHwnd, [ref]$dropPt) | Out-Null
[NativeUi]::SetCursorPos($dropPt.X, $dropPt.Y) | Out-Null
Start-Sleep -Milliseconds 120
Invoke-PressEscape

Start-Sleep -Milliseconds 220
Save-ActiveWindowShot -Hwnd $mainHwnd -Path $afterPath

$statusHwnd = Get-ChildByClass -Parent $mainHwnd -ClassName 'msctls_statusbar32'
if ($statusHwnd -ne [IntPtr]::Zero) {
    Save-ChildShot -Hwnd $statusHwnd -Path $statusPath
}

if (-not $proc.HasExited) {
    $null = $proc.CloseMainWindow()
    Start-Sleep -Milliseconds 450
    if (-not $proc.HasExited) {
        $proc.Kill()
    }
}

Write-Output "before=$beforePath"
Write-Output "after=$afterPath"
Write-Output "status=$statusPath"
