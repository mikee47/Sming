REM Windows install script

rmdir /s /q c:\MinGW
curl -LO %SMINGTOOLS%/MinGW.7z
7z -oC:\ x MinGW.7z

goto :%SMING_ARCH%

:Esp8266

	REM Old toolchain
	set TOOLCHAIN=esp-udk-win32.7z
	curl -LO %SMINGTOOLS%/%TOOLCHAIN%
	7z -o%UDK_ROOT% x %TOOLCHAIN%

	REM New toolchain
	mkdir %EQT_ROOT%
	set TOOLCHAIN=i686-w64-mingw32.xtensa-lx106-elf-a5c9861.1575819473.zip
	curl -LO https://github.com/earlephilhower/esp-quick-toolchain/releases/download/3.0.0-gnu5/%TOOLCHAIN%
	7z -o%EQT_ROOT% x %TOOLCHAIN%

	goto :EOF


:Host

	REM Fix issue with "couldn't commit memory for cygwin heap" error - precise culprit in the path TBD
	setx /m path "c:\mingw\bin;c:\mingw\msys\1.0\bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\7-Zip;C:\Program Files (x86)\CMake\bin;C:\Python27;C:\Python27\Scripts;C:\Tools\curl\bin;C:\Program Files\PowerShell\6\;C:\Program Files\Git\cmd;C:\Program Files\Git\usr\bin"

	goto :EOF

