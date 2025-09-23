@ECHO OFF
CHCP 65001 > NUL
SETLOCAL ENABLEDELAYEDEXPANSION

WHERE cmake >NUL 2>NUL
IF %ERRORLEVEL% EQU 0 GOTO lbMain





REM "cmake" not callable without giving a path --> try to initialize the VS Dev CMD environment
ECHO [Trying to initialize the "Developer Command Prompt for Visual Studio" environment...]
SET "sVSWHERE_EXE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

IF NOT EXIST "%sVSWHERE_EXE%" GOTO lbError_NoCMake

SET "sDevCmd="
FOR /F "USEBACKQ TOKENS=*" %%i IN (`"%sVSWHERE_EXE%" -latest -requires Microsoft.VisualStudio.Component.VC.CMake.Project -find **\VsDevCmd.bat`) DO (
    SET "sDevCmd=%%i"
)
IF "!sDevCmd!" EQU "" GOTO lbError_NoCMake

CALL "!sDevCmd!" > NUL
ECHO.  Succeeded^^!
ECHO.
GOTO lbMain

:lbError_NoCMake
ECHO Error: cmake.exe couldn't be found.
ECHO Add it to PATH or call this script within the "Developer Command Prompt for Visual Studio".
EXIT /b 1





:lbMain

IF NOT EXIST out MKDIR out > NUL

CD out

ECHO ╔═[Processing preset win-x64-release...]═══════════════════════════════════════╗
cmake .. --preset win-x64-release > NUL
cmake --build build\win-x64-release > NUL

ECHO ╠═[Processing preset win-x86-release...]═══════════════════════════════════════╣
cmake .. --preset win-x86-release > NUL
cmake --build build\win-x86-release > NUL


ECHO ╚══════════════════════════════════════════════════════════════════════════════╝

CD ..





:lbEnd
EXIT /b 0
