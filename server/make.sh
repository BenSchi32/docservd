#!/bin/bash

if [ "$1" != "server_main.asm" ]; then
	nasm -f elf64 -g -F stabs $1.asm
	ld -g -o $1 $1.o
	./$1
else
	echo "Filename without .asm"
fi
