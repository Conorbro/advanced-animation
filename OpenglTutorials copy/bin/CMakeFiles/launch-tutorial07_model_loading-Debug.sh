#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial07_model_loading/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/Debug/tutorial07_model_loading 
	else
		"/Users/conorbroderick/Opengl/bin/Debug/tutorial07_model_loading"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/Debug/tutorial07_model_loading"  
fi
