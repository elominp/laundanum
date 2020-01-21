#!/usr/bin/env bash

gcc -Os main.c -o buffer_overflow
gcc -Os -S main.c
chmod +x buffer_overflow
./buffer_overflow