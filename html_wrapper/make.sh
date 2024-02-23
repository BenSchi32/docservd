#!/bin/bash

gcc -o wrap main.c /home/dev/c/my_libs/kmp/kmp.c
./wrap $1
