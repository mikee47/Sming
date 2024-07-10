REM @echo off

REM
REM Windows install script
REM

set

where /q winget
if errorlevel 1 (
    echo Winget is required: See https://learn.microsoft.com/en-us/windows/package-manager/winget/
    goto :EOF
)

REM Install required standard packages
winget install --silent --no-upgrade --accept-package-agreements --source winget ^
    git.git ^
    python.python.3.12 ^
    ninja-build.ninja ^
    ccache.ccache ^
    7zip.7zip

set DOWNLOADS=downloads
if not exist "%DOWNLOADS%" (
    mkdir %DOWNLOADS%
)

set SMINGTOOLS=https://github.com/SmingHub/SmingTools/releases/download/1.0

REM Leave file endings alone
git config --global --add core.autocrlf input

echo.
echo.
echo ** Installing common python requirements
echo.
python -m pip install --upgrade pip -r %SMING_HOME%\..\Tools\requirements.txt

if exist "C:\MinGW\bin" goto :mingw_installed

echo.
echo.
echo ** Installing MinGW
echo.
rmdir /s /q c:\MinGW
curl -Lo %DOWNLOADS%\MinGW.7z %SMINGTOOLS%/MinGW-2020-10-19.7z
7z -oC:\ x %DOWNLOADS%\MinGW.7z

goto :install

:mingw_installed
echo.
echo ** MinGW found, not re-installing
echo.

:install

if "%1" == "" goto :EOF
if "%1" == "all" (
    call :install Host Esp8266 Esp32 Rp2040
) else (
    echo.
    echo.
    echo ** Installing %1 toolchain
    echo.
    if "%1" == "doc" (
        call %SMING_HOME%\..\docs\Tools\install.cmd
    ) else (
        call %SMING_HOME%\Arch\%1\Tools\install.cmd
    )
)
shift
goto :install
