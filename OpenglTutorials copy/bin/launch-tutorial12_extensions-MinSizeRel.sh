#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/tutorial12_extensions/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/MinSizeRel/tutorial12_extensions 
	else
		"/Users/conorbroderick/Opengl/bin/MinSizeRel/tutorial12_extensions"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/MinSizeRel/tutorial12_extensions"  
fi
