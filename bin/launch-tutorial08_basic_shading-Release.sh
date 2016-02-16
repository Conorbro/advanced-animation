#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial08_basic_shading/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/Release/tutorial08_basic_shading 
	else
		"/Users/conorbroderick/Opengl/bin/Release/tutorial08_basic_shading"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/Release/tutorial08_basic_shading"  
fi
