#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial10_transparency/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial10_transparency 
	else
		"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial10_transparency"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial10_transparency"  
fi
