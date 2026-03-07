[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ExePath,
    [Parameter(Mandatory = $true)]
    [string]$Tag,
    [ValidateSet(100, 125, 150)]
    [int]$DpiScale = 100,
    [string]$OutputDir = ""
)

$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.Drawing

$nativeCode = @"
using System;
using System.Runtime.InteropServices;

public static class UiSmokeNative {
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }

    [StructLayout(LayoutKind.Sequential)]
    public struct GUITHREADINFO {
        public int cbSize;
        public int flags;
        public IntPtr hwndActive;
        public IntPtr hwndFocus;
        public IntPtr hwndCapture;
        public IntPtr hwndMenuOwner;
        public IntPtr hwndMoveSize;
        public IntPtr hwndCaret;
        public RECT rcCaret;
    }

    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr hWndParent, EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [DllImport("user32.dll")] public static extern bool GetGUIThreadInfo(uint idThread, ref GUITHREADINFO lpgui);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int X, int Y);
    [DllImport("user32.dll")] public static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint dwData, UIntPtr dwExtraInfo);
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr hWnd, IntPtr hdcBlt, uint nFlags);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern IntPtr SendMessageW(IntPtr hWnd, uint msg, IntPtr wParam, string lParam);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern int GetClassNameW(IntPtr hWnd, System.Text.StringBuilder className, int maxCount);

    public const int SW_RESTORE = 9;
    public const uint SWP_NOZORDER = 0x0004;
    public const uint SWP_NOACTIVATE = 0x0010;
    public const uint MOUSEEVENTF_LEFTDOWN = 0x0002;
    public const uint MOUSEEVENTF_LEFTUP = 0x0004;
    public const uint WM_SETTEXT = 0x000C;
}
"@

Add-Type -TypeDefinition $nativeCode -ReferencedAssemblies "System.dll"

function Wait-MainWindowProcess {
    param(
        [Parameter(Mandatory = $true)][System.Diagnostics.Process]$Process,
        [int]$TimeoutMs = 15000
    )

    $deadline = (Get-Date).AddMilliseconds($TimeoutMs)
    while ((Get-Date) -lt $deadline) {
        $candidate = Get-Process -Id $Process.Id -ErrorAction SilentlyContinue
        if ($candidate -ne $null -and $candidate.MainWindowHandle -ne 0) {
            return $candidate
        }
        Start-Sleep -Milliseconds 120
    }

    return $null
}

function Stop-ProcessSafe {
    param([System.Diagnostics.Process]$Process)

    if ($Process -eq $null) {
        return
    }

    try {
        if (-not $Process.HasExited) {
            $null = $Process.CloseMainWindow()
            Start-Sleep -Milliseconds 300
        }
    }
    catch {
    }

    try {
        if (-not $Process.HasExited) {
            $Process.Kill()
            $Process.WaitForExit(2000) | Out-Null
        }
    }
    catch {
    }
}

function Prepare-Window {
    param(
        [Parameter(Mandatory = $true)][IntPtr]$Hwnd,
        [int]$Width,
        [int]$Height
    )

    [UiSmokeNative]::ShowWindow($Hwnd, [UiSmokeNative]::SW_RESTORE) | Out-Null
    [UiSmokeNative]::SetWindowPos(
        $Hwnd,
        [IntPtr]::Zero,
        60,
        60,
        $Width,
        $Height,
        [UiSmokeNative]::SWP_NOZORDER -bor [UiSmokeNative]::SWP_NOACTIVATE) | Out-Null
    [UiSmokeNative]::SetForegroundWindow($Hwnd) | Out-Null
    Start-Sleep -Milliseconds 260
}

function Click-Editor {
    param(
        [Parameter(Mandatory = $true)][IntPtr]$Hwnd,
        [int]$DpiScaleValue
    )

    [UiSmokeNative+RECT]$rect = New-Object UiSmokeNative+RECT
    [UiSmokeNative]::GetWindowRect($Hwnd, [ref]$rect) | Out-Null

    $scaledX = [Math]::Round(230.0 * $DpiScaleValue / 100.0)
    $scaledY = [Math]::Round(220.0 * $DpiScaleValue / 100.0)
    $targetX = $rect.Left + [int]$scaledX
    $targetY = $rect.Top + [int]$scaledY

    [UiSmokeNative]::SetCursorPos($targetX, $targetY) | Out-Null
    Start-Sleep -Milliseconds 30
    [UiSmokeNative]::mouse_event([UiSmokeNative]::MOUSEEVENTF_LEFTDOWN, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 20
    [UiSmokeNative]::mouse_event([UiSmokeNative]::MOUSEEVENTF_LEFTUP, 0, 0, 0, [UIntPtr]::Zero)
    Start-Sleep -Milliseconds 120
}

function Get-WindowClassName {
    param([IntPtr]$Hwnd)

    if ($Hwnd -eq [IntPtr]::Zero) {
        return ""
    }

    $classBuilder = New-Object System.Text.StringBuilder 256
    [void][UiSmokeNative]::GetClassNameW($Hwnd, $classBuilder, $classBuilder.Capacity)
    return $classBuilder.ToString()
}

function Get-FocusedChildWindow {
    param([IntPtr]$MainHwnd)

    [uint32]$processId = 0
    $threadId = [UiSmokeNative]::GetWindowThreadProcessId($MainHwnd, [ref]$processId)
    if ($threadId -eq 0) {
        return [IntPtr]::Zero
    }

    $guiInfo = New-Object UiSmokeNative+GUITHREADINFO
    $guiInfo.cbSize = [Runtime.InteropServices.Marshal]::SizeOf([type]([UiSmokeNative+GUITHREADINFO]))
    if (-not [UiSmokeNative]::GetGUIThreadInfo([uint32]$threadId, [ref]$guiInfo)) {
        return [IntPtr]::Zero
    }

    return $guiInfo.hwndFocus
}

function Find-LargestEditDescendant {
    param([IntPtr]$MainHwnd)

    $script:uiSmokeBestEditHwnd = [IntPtr]::Zero
    $script:uiSmokeBestEditArea = -1
    [UiSmokeNative]::EnumChildWindows($MainHwnd, {
        param($childHwnd, $lParam)
        $className = Get-WindowClassName -Hwnd $childHwnd
        if ($className.IndexOf("Edit", [System.StringComparison]::OrdinalIgnoreCase) -ge 0 -or
            $className.IndexOf("RichEdit", [System.StringComparison]::OrdinalIgnoreCase) -ge 0) {
            [UiSmokeNative+RECT]$childRect = New-Object UiSmokeNative+RECT
            [UiSmokeNative]::GetWindowRect($childHwnd, [ref]$childRect) | Out-Null
            $width = $childRect.Right - $childRect.Left
            $height = $childRect.Bottom - $childRect.Top
            $area = $width * $height
            if ($area -gt $script:uiSmokeBestEditArea) {
                $script:uiSmokeBestEditArea = $area
                $script:uiSmokeBestEditHwnd = $childHwnd
            }
        }
        return $true
    }, [IntPtr]::Zero) | Out-Null

    return $script:uiSmokeBestEditHwnd
}

function Resolve-EditorHandle {
    param(
        [IntPtr]$MainHwnd,
        [int]$DpiScaleValue
    )

    [UiSmokeNative]::SetForegroundWindow($MainHwnd) | Out-Null
    Start-Sleep -Milliseconds 140
    Click-Editor -Hwnd $MainHwnd -DpiScaleValue $DpiScaleValue

    $focusedHwnd = Get-FocusedChildWindow -MainHwnd $MainHwnd
    if ($focusedHwnd -eq [IntPtr]::Zero) {
        throw "Unable to resolve focused editor window."
    }

    $focusedClass = Get-WindowClassName -Hwnd $focusedHwnd
    if ($focusedClass.IndexOf("Edit", [System.StringComparison]::OrdinalIgnoreCase) -ge 0 -or
        $focusedClass.IndexOf("RichEdit", [System.StringComparison]::OrdinalIgnoreCase) -ge 0) {
        return $focusedHwnd
    }

    $editDescendant = Find-LargestEditDescendant -MainHwnd $MainHwnd
    if ($editDescendant -ne [IntPtr]::Zero) {
        return $editDescendant
    }

    throw "Focused control is not an edit control (class='$focusedClass') and no edit descendants were found."
}

function Set-EditorText {
    param(
        [IntPtr]$EditorHwnd,
        [string]$Text
    )

    [void][UiSmokeNative]::SendMessageW($EditorHwnd, [UiSmokeNative]::WM_SETTEXT, [IntPtr]::Zero, $Text)
    Start-Sleep -Milliseconds 220
}

function Save-WindowShot {
    param(
        [Parameter(Mandatory = $true)][IntPtr]$Hwnd,
        [Parameter(Mandatory = $true)][string]$Path
    )

    [UiSmokeNative+RECT]$rect = New-Object UiSmokeNative+RECT
    [UiSmokeNative]::GetWindowRect($Hwnd, [ref]$rect) | Out-Null
    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    if ($width -le 1 -or $height -le 1) {
        throw "Invalid window dimensions for screenshot: ${width}x${height}"
    }

    $bitmap = New-Object System.Drawing.Bitmap($width, $height, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $hdc = $graphics.GetHdc()
    [void][UiSmokeNative]::PrintWindow($Hwnd, $hdc, 2)
    $graphics.ReleaseHdc($hdc)
    $graphics.Dispose()
    $bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bitmap.Dispose()
}

function Get-ImageDiffRatio {
    param(
        [Parameter(Mandatory = $true)][string]$FirstImagePath,
        [Parameter(Mandatory = $true)][string]$SecondImagePath,
        [int]$SampleStep = 2,
        [int]$PixelThreshold = 12
    )

    $firstBitmap = New-Object System.Drawing.Bitmap($FirstImagePath)
    $secondBitmap = New-Object System.Drawing.Bitmap($SecondImagePath)
    try {
        if ($firstBitmap.Width -ne $secondBitmap.Width -or $firstBitmap.Height -ne $secondBitmap.Height) {
            return 1.0
        }

        $diffPixels = 0
        $sampleCount = 0
        for ($y = 0; $y -lt $firstBitmap.Height; $y += $SampleStep) {
            for ($x = 0; $x -lt $firstBitmap.Width; $x += $SampleStep) {
                $c1 = $firstBitmap.GetPixel($x, $y)
                $c2 = $secondBitmap.GetPixel($x, $y)
                $delta = [Math]::Abs($c1.R - $c2.R) + [Math]::Abs($c1.G - $c2.G) + [Math]::Abs($c1.B - $c2.B)
                if ($delta -gt $PixelThreshold) {
                    $diffPixels++
                }
                $sampleCount++
            }
        }

        if ($sampleCount -le 0) {
            return 0.0
        }

        return [double]$diffPixels / [double]$sampleCount
    }
    finally {
        $firstBitmap.Dispose()
        $secondBitmap.Dispose()
    }
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$resolvedExePath = if ([System.IO.Path]::IsPathRooted($ExePath)) {
    $ExePath
}
else {
    Join-Path $repoRoot $ExePath
}

if (-not (Test-Path -LiteralPath $resolvedExePath)) {
    throw "Executable not found: $resolvedExePath"
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

$shortPath = Join-Path $resolvedOutputDir ("ui-smoke-{0}-dpi{1}-short.png" -f $Tag, $DpiScale)
$overflowPath = Join-Path $resolvedOutputDir ("ui-smoke-{0}-dpi{1}-overflow.png" -f $Tag, $DpiScale)
$backPath = Join-Path $resolvedOutputDir ("ui-smoke-{0}-dpi{1}-back.png" -f $Tag, $DpiScale)

$windowWidth = [Math]::Max(840, [int]([Math]::Round(1180.0 * $DpiScale / 100.0)))
$windowHeightLarge = [Math]::Max(620, [int]([Math]::Round(760.0 * $DpiScale / 100.0)))
$windowHeightSmall = [Math]::Max(260, [int]([Math]::Round($windowHeightLarge * 0.38)))

$stableThreshold = 0.003
$changedThreshold = 0.900

$process = $null
try {
    $process = Start-Process -FilePath $resolvedExePath -PassThru
    Start-Sleep -Milliseconds 1100

    $mainWindowProcess = Wait-MainWindowProcess -Process $process
    if ($mainWindowProcess -eq $null) {
        throw "Main window was not detected for process $($process.Id)."
    }

    $mainHwnd = [IntPtr]$mainWindowProcess.MainWindowHandle
    Prepare-Window -Hwnd $mainHwnd -Width $windowWidth -Height $windowHeightLarge
    Save-WindowShot -Hwnd $mainHwnd -Path $shortPath

    Prepare-Window -Hwnd $mainHwnd -Width $windowWidth -Height $windowHeightSmall
    Save-WindowShot -Hwnd $mainHwnd -Path $overflowPath

    Prepare-Window -Hwnd $mainHwnd -Width $windowWidth -Height $windowHeightLarge
    Save-WindowShot -Hwnd $mainHwnd -Path $backPath

    $diffShortOverflow = Get-ImageDiffRatio -FirstImagePath $shortPath -SecondImagePath $overflowPath
    $diffShortBack = Get-ImageDiffRatio -FirstImagePath $shortPath -SecondImagePath $backPath
    $passed = ($diffShortOverflow -ge $changedThreshold) -and ($diffShortBack -le $stableThreshold)

    $summary = [ordered]@{
        tag = $Tag
        dpi = $DpiScale
        exe = $resolvedExePath
        short = $shortPath
        overflow = $overflowPath
        back = $backPath
        diff_short_overflow = [Math]::Round($diffShortOverflow, 6)
        diff_short_back = [Math]::Round($diffShortBack, 6)
        threshold_changed = $changedThreshold
        threshold_stable = $stableThreshold
        passed = $passed
    }

    $summaryPath = Join-Path $resolvedOutputDir ("ui-smoke-{0}-dpi{1}-summary.json" -f $Tag, $DpiScale)
    $summary | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $summaryPath -Encoding UTF8
    Write-Output ($summary | ConvertTo-Json -Compress)

    if (-not $passed) {
        throw ("Visual diff thresholds failed for tag={0}, dpi={1}: changed={2}, stable={3}" -f
            $Tag, $DpiScale, [Math]::Round($diffShortOverflow, 6), [Math]::Round($diffShortBack, 6))
    }
}
finally {
    Stop-ProcessSafe -Process $process
}
