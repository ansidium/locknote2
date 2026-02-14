# Changelog

## 2.1.1 - 2026-02-14

### Security
- Migrated file path operations to Unicode-safe WinAPI variants (`BeginUpdateResourceW`, `UpdateResourceW`, `CopyFileW`, `_wfopen_s`) to avoid ANSI/UTF-8 path corruption.
- Added bounded retry loops for helper writeback/erase operations to prevent unbounded waits on file lock failures.
- Added constant-time password comparison in the password change flow.

### Reliability
- Added strict error handling for module path retrieval, temp path creation, and helper process spawn failures.
- Normalized drag-and-drop filename handling to UTF-16 (`DragQueryFileW`) and UTF-8 conversion.

### Build and Toolchain
- Set project language mode to `/std:c++23preview`.
- Raised warning level to `/W4` and enabled UTF-8 source handling (`/utf-8`).

### QA
- Added `scripts/windows-qa.ps1` for Debug/Release builds, optional code analysis, optional cppcheck, and smoke tests.
- Added `scripts/build-and-run-aes-smoke.ps1`.
- Added `tests/aeslayer_smoke.cpp` with round-trip and tamper-detection checks.

### Upstream tracking
- Verified PR #6 (`steganos-oss/locknote2#6`) integration in this fork.
- Reviewed upstream open issues #3, #4, #5, #7, #8 and revalidated corresponding areas for regressions.
