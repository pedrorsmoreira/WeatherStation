#!/bin/sh

if [ "$1" = "" ]; then
	cp /media/sce/KINGSTON/SCE/communication.c communication.c
	cp /media/sce/KINGSTON/SCE/Makefile Makefile
	cp /media/sce/KINGSTON/SCE/processing.c processing.c
	cp /media/sce/KINGSTON/SCE/user.c user.c
	cp /media/sce/KINGSTON/SCE/user_commands.c user_commands.c
	cp /media/sce/KINGSTON/SCE/wsremote.c wsremote.c
	cp /media/sce/KINGSTON/SCE/structure.h structure.h
	cp /media/sce/KINGSTON/SCE/structure.c structure.c
else
	echo "compile from current files"
fi

make INSTALL_DIR=../../pc_install clean
make INSTALL_DIR=../../pc_install #| fgrep -v 'i386-elf-gcc'| fgrep -v 'C++/ObjC++'

if [ "$?" = "0" ]; then
	i386-elf-objcopy -O binary wsremote wsremote.bin
	echo "i386 bin done"
	dd conv=sync if=wsremote.bin of=/dev/fd0
	echo "done!"
else
	echo "abort!"
	exit 1
fi
