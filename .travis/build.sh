#!/bin/bash
set -ex # exit with nonzero exit code if anything fails

# Setup ARCH SDK
export SMING_HOME=$TRAVIS_BUILD_DIR/Sming
cd $SMING_HOME

# Build times benefit from parallel building
export MAKE_PARALLEL="make -j3"

TRAVIS_OSDIR="$TRAVIS_BUILD_DIR/.travis/$TRAVIS_OS_NAME"
TRAVIS_ARCHDIR="$SMING_HOME/Arch/$SMING_ARCH/Tools/travis/$TRAVIS_OS_NAME"

SOURCE="$TRAVIS_OSDIR/build.setup.sh"
if [ -f "$SOURCE" ]; then
       source "$SOURCE"
fi

SOURCE="$TRAVIS_ARCHDIR/build.setup.sh"
if [ -f "$SOURCE" ]; then
       source "$SOURCE"
fi

env

# Full compile checks please
export STRICT=1

# Move samples and tests into directory outside of the Sming repo.
export SMING_PROJECTS_DIR=$HOME/projects
mkdir $SMING_PROJECTS_DIR
mv ../samples $SMING_PROJECTS_DIR
mv ../tests $SMING_PROJECTS_DIR

# Diagnostic info
cd $SMING_PROJECTS_DIR/samples/Basic_Blink
make help
make list-config

# Run ARCH SDK tests
cd $SMING_HOME

SOURCE="$TRAVIS_OSDIR/build.run.sh"
if [ -f "$SOURCE" ]; then
       source "$SOURCE"
fi

SOURCE="$TRAVIS_ARCHDIR/build.run.sh"
if [ -f "$SOURCE" ]; then
       source "$SOURCE"
fi
