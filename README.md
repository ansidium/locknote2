## Steganos LockNote 2

Self-modifying encrypted notepad  
Copyright (C) 2024 Steganos Software GmbH

Steganos LockNote 2 is the modern continuation of the original LockNote (2006).  
It stores encrypted text directly inside the executable file, so the program is the document.

No installation is required, and no external data folder is used.

## Official Links

- Product page: <https://www.steganos.com/en/products/steganos-locknote-2>
- Upstream source code: <https://github.com/steganos-oss/locknote2>
- Official binary: <https://go.steganos.com/productpage_locknote2_download_en>

## Features

- Portable single-file encrypted notes
- Modern crypto stack (AES-CBC + HMAC-SHA256)
- Password derivation via scrypt (optional PBKDF2 profile)
- Multi-language UI
- High-DPI support
- Classic Notepad-like workflow

## Build Requirements (Windows)

- Microsoft Visual Studio 2022 17.14+ (`v143`)
- Workload: `Desktop development with C++`
- Components:
  - `MSVC v143 - VS 2022 C++ x64/x86 build tools`
  - `Windows 11 SDK` (10.0.26100+ recommended)
  - `C++ ATL for latest v143 build tools` (required by ATL/WTL)
- Git for Windows
- PowerShell 7 (`pwsh`) for QA scripts

This project uses:

- C++20 by default (C++23 preview is optional)
- vcpkg manifest mode (`vcpkg.json`)

## One-Time vcpkg Setup

```powershell
git clone https://github.com/microsoft/vcpkg $env:USERPROFILE\vcpkg
& $env:USERPROFILE\vcpkg\bootstrap-vcpkg.bat -disableMetrics
$env:VCPKG_ROOT = "$env:USERPROFILE\vcpkg"
& $env:VCPKG_ROOT\vcpkg integrate install
```

## Build from Command Line

### Win32 Release

```powershell
msbuild .\locknote2.sln /m /t:Restore,Build /p:Configuration=Release;Platform=Win32;VcpkgEnableManifest=true
```

### x64 Release

```powershell
msbuild .\locknote2.sln /m /t:Restore,Build /p:Configuration=Release;Platform=x64;VcpkgEnableManifest=true
```

### Optional C++23 Preview Mode

```powershell
msbuild .\locknote2.sln /m /t:Build /p:Configuration=Release;Platform=Win32;LockNoteCppLanguageStandard=stdcpp23preview
```

## Dependencies

- CryptoPP (via vcpkg manifest)
- WTL 10 (via vcpkg manifest)

Dependencies are restored automatically when vcpkg integration is enabled.

## Local QA

```powershell
pwsh .\scripts\windows-qa.ps1
```

The script runs:

- Debug/Release builds (`Win32`)
- Optional MSVC code analysis (`RunCodeAnalysis=true`)
- Optional `cppcheck` (if installed)
- AES encryption/decryption smoke tests (`tests/aeslayer_smoke.cpp`)

## CI

GitHub Actions workflow:

- `.github/workflows/windows-qa.yml`

## Changelog and History

- Full release history and change details are maintained in `CHANGELOG.md`.

## License

This project is licensed under the GNU General Public License, version 2 or later (GPL-2.0-or-later).  
See `LICENSE` for full text.
