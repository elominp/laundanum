#!/usr/bin/env bash

gcc -Os main.c -o stack_overflow
gcc -Os -S main.c
chmod +x stack_overflow
./stack_overflow