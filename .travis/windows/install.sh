# Windows installation

choco uninstall -y mingw

rm -rf /C/MinGW
curl -Lo MinGW.7z $SMINGTOOLS/MinGW-2020-10-19.7z
7z -o/C/ x MinGW.7z

choco install python --version=3.8.6
export PATH=/C/Python38/:$PATH
