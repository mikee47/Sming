@echo off

set CUR_DIR=%CD%
cd %~dp0..\Sming
set SMING_HOME=%CD%
cd %CUR_DIR%

if "%TOOLS_DIR%"=="" (
    set TOOLS_DIR=C:\tools
)

REM Esp8266 
set ESP_HOME=%TOOLS_DIR%\esp-quick-toolchain

REM Esp32
set IDF_PATH=%TOOLS_DIR%\esp-idf
set IDF_TOOLS_PATH=%TOOLS_DIR%\esp32

REM Rp2040
set PICO_TOOLCHAIN_PATH=%TOOLS_DIR%\rp2040

where /q python
if errorlevel 1 (
    echo ERROR! Python not found in path.
    goto :EOF
)

where /q mingw-get
if errorlevel 1 (
    python "%~dp0update-path.py" C:\MinGW\msys\1.0\bin C:\MinGW\bin > new_path.lst
    if errorlevel 1 (
        del new_path.lst
        goto :EOF
    )
    for /f "delims==" %%a in (new_path.lst) do (
      path %%a
    )
    del new_path.lst
)

where /q cmake
if errorlevel 1 (
    echo WARNING! Cmake not found in path.
)

where /q ninja
if errorlevel 1 (
    echo WARNING! Ninja not found in path.
)
