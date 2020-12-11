#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

TRAVIS_OSDIR="$TRAVIS_BUILD_DIR/.travis/$TRAVIS_OS_NAME"
TRAVIS_ARCHDIR="$TRAVIS_BUILD_DIR/Sming/Arch/$SMING_ARCH/Tools/travis/$TRAVIS_OS_NAME"

SOURCE="$TRAVIS_OSDIR/install.sh"
if [ -f "$SOURCE" ]; then
    source "$SOURCE"
fi

SOURCE="$TRAVIS_ARCHDIR/install.sh"
if [ -f "$SOURCE" ]; then
    source "$SOURCE"
fi
