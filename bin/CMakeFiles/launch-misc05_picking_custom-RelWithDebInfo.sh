#!/bin/sh
bindir=$(pwd)
cd /Users/conorbroderick/Opengl/OpenglTutorials/misc05_picking/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /Users/conorbroderick/Opengl/bin/RelWithDebInfo/misc05_picking_custom 
	else
		"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/misc05_picking_custom"  
	fi
else
	"/Users/conorbroderick/Opengl/bin/RelWithDebInfo/misc05_picking_custom"  
fi
