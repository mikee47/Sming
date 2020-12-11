# Windows installation

rm -rf /C/MinGW
curl -LO $SMINGTOOLS/MinGW.7z
7z -o/C/ x MinGW.7z

choco install python --version=3.8.6
export PATH=/C/Python38/:$PATH
