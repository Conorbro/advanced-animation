#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial11_2d_fonts/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/MinSizeRel/tutorial11_2d_fonts 
	else
		"/Users/conorbroderick/Opengl/bin/MinSizeRel/tutorial11_2d_fonts"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/MinSizeRel/tutorial11_2d_fonts"  
fi
