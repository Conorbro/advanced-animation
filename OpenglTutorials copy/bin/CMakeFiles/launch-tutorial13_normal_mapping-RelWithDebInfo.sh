#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial13_normal_mapping/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial13_normal_mapping 
	else
		"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial13_normal_mapping"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/tutorial13_normal_mapping"  
fi
