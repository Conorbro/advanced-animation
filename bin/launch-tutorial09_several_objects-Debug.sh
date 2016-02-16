#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial09_vbo_indexing/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/Debug/tutorial09_several_objects 
	else
		"/Users/conorbroderick/Opengl/bin/Debug/tutorial09_several_objects"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/Debug/tutorial09_several_objects"  
fi
