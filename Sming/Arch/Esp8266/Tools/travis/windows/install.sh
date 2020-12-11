echo "Installing Windows Toolchains"

if [ "$BUILD_COMPILER" == "udk" ]; then
    echo "Old toolchain"
    export TOOLCHAIN=esp-udk-win32.7z
    curl -LO "$SMINGTOOLS/$TOOLCHAIN"
    7z -o"$UDK_ROOT" x $TOOLCHAIN
fi

if [ "$BUILD_COMPILER" == "eqt" ]; then
    echo "New toolchain"
    mkdir -p "$EQT_ROOT"
    export TOOLCHAIN=i686-w64-mingw32.xtensa-lx106-elf-a5c9861.1575819473.zip
    curl -LO "https://github.com/earlephilhower/esp-quick-toolchain/releases/download/3.0.0-gnu5/$TOOLCHAIN"
    7z -o"$EQT_ROOT" x $TOOLCHAIN
fi

echo "Done."
