export PATH=/C/MinGW/msys/1.0/bin:/C/MinGW/bin:/C/Python38:$PATH
export PYTHON='/C/Python38/python'

ln -s $TRAVIS_BUILD_DIR /c/z
export SMING_HOME=/c/z/Sming
cd $SMING_HOME
