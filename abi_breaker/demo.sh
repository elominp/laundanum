#!/usr/bin/env bash

# Build libfillBoom
gcc -c -Os -fPIC fill_boom.c
gcc -S -Os -fPIC fill_boom.c
gcc -Os -shared -fPIC fill_boom.o -o libfillBoom.so

# Build abi_breaker
gcc -Os -S main.c
gcc -Os -o abi_breaker main.c -lfillBoom -L.

# Execute abi_breaker
chmod +x abi_breaker
./abi_breaker