#!/bin/bash

if [ "$1" != "server_main.asm" ]; then
	nasm -f elf64 -o docservd.o -g -F stabs $1.asm
	ld -g -o docservd docservd.o
	./docservd
else
	echo "Filename without .asm"
fi
