REM @echo off

REM
REM Windows install script
REM

where /q choco
if errorlevel 1 (
    @powershell -NoProfile -ExecutionPolicy unrestricted -Command "iex ((new-object net.webclient).DownloadString('https://chocolatey.org/install.ps1'))"
    if errorlevel 1 goto :EOF
)

REM Install required standard packages
set PKGLIST=
where git
if errorlevel 1 PKGLIST=%PKGLIST% git
where python
if errorlevel 1 PKGLIST=%PKGLIST% python
where ninja
if errorlevel 1 PKGLIST=%PKGLIST% ninja-build
where ccache
if errorlevel 1 PKGLIST=%PKGLIST% ccache
where 7z
if errorlevel 1 PKGLIST=%PKGLIST% 7zip
where cmake
if errorlevel 1 PKGLIST=%PKGLIST% cmake
if "%PKGLIST%" NEQ "" choco install %PKGLIST% -yr

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
if exist c:\MinGW rmdir /s /q c:\MinGW
curl -Lo %DOWNLOADS%\MinGW.7z %SMINGTOOLS%/MinGW-2020-10-19.7z
7z -oC:\ x %DOWNLOADS%\MinGW.7z

goto :install

:mingw_installed
echo.
echo ** MinGW found, not re-installing
echo.

:install

@powershell -NoProfile -ExecutionPolicy unrestricted update-path.ps1 "C:\MinGW\bin,C:\MinGW\msys\1.0\bin"

call %~dp0export.cmd

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
