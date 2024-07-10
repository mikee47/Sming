REM
REM Windows CI install script
REM

set TOOLS_DIR=D:\opt

cd "%~dp0"
call ..\export.cmd

if "%BUILD_DOCS%" == "true" (
    set INSTALL_OPTS=doc
)

call ..\install.cmd %SMING_ARCH% %INSTALL_OPTS%

REM Configure ccache
ccache --set-config cache_dir="%CI_BUILD_DIR%\.ccache"
ccache --set-config max_size=500M
ccache -z

REM Clean up tools installation
python clean-tools.py clean --delete
