REM Docs install.cmd

where /q doxygen
if errorlevel 1 (
    winget install doxygen graphviz
)

python -m pip install --upgrade pip wheel
python -m pip install -r %~dp0/../requirements.txt
python -m pip uninstall -y xcffib
