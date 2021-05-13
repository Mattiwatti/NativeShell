@echo off

:: set WDKDIR=C:\Dev\WinDDK\3790.1830
:: set WDKDIR=C:\Dev\WinDDK\6000
set WDKDIR=C:\Dev\WinDDK\7600.16385.1
set ACTUAL_RECENTISH_SDK_INCLUDE_DIR_WITH_NO_SAL2_H_IN_IT=C:\Program Files (x86)\Windows Kits\8.0\Include\shared

if not "x%WDKDIR%" == "x" goto SELECT_EXE
echo The WDKDIR environment variable is not set
echo Set this variable to your WDK directory (without ending backslash)
echo Example: set WDKDIR C:\WinDDK\6001
pause
goto:eof

:SELECT_EXE
set PROJECT_DIR=%~dp0
set EXE_NAME=NativeShell

:BUILD_EXE_64
setlocal
echo Building %EXE_NAME%.exe (64-bit) ...
set DDKBUILDENV=

:: for DDK 3790/6000:
:: call %WDKDIR%\bin\setenv.bat %WDKDIR%\ fre AMD64 WNET

:: for DDK 7601:
call %WDKDIR%\bin\setenv.bat %WDKDIR%\ fre x64 WNET no_oacr
cd %PROJECT_DIR%

set INCLUDE=%INCLUDE%;%PROJECT_DIR%\ndk;%ACTUAL_RECENTISH_SDK_INCLUDE_DIR_WITH_NO_SAL2_H_IN_IT%

build /c /z /g /w /M 8 /nosqm
del buildfre_wnet_amd64.log 1>nul 2>&1
echo.
endlocal

:BUILD_EXE_32
setlocal
echo Building %EXE_NAME%.exe (32-bit) ...
set DDKBUILDENV=

:: for DDK 3790/6000:
::call %WDKDIR%\bin\setenv.bat %WDKDIR%\ fre WNET

:: for DDK 7601:
call %WDKDIR%\bin\setenv.bat %WDKDIR%\ fre WNET no_oacr
cd %PROJECT_DIR%

set INCLUDE=%INCLUDE%;%PROJECT_DIR%\ndk;%ACTUAL_RECENTISH_SDK_INCLUDE_DIR_WITH_NO_SAL2_H_IN_IT%

build /c /z /g /w /M 8 /nosqm
del buildfre_wnet_x86.log 1>nul 2>&1
echo.
endlocal
