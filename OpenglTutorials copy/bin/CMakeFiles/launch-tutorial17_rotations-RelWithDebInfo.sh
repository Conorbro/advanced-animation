#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial17_rotations/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial17_rotations 
	else
		"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial17_rotations"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial17_rotations"  
fi
