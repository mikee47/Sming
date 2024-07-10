REM
REM Windows CI install script
REM

set TOOLS_DIR=D:\opt

if "%BUILD_DOCS%" == "true" (
    set INSTALL_OPTS=doc
)

call "%~dp0\..\install.cmd" %SMING_ARCH% %INSTALL_OPTS%
if errorlevel 1 goto :EOF

REM Configure ccache
ccache --set-config cache_dir="%CI_BUILD_DIR%\.ccache"
ccache --set-config max_size=500M
ccache -z

REM Clean up tools installation
python clean-tools.py clean --delete
