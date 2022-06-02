#!/bin/sh -e

gcc -std=c89 -Os -o way.o -c way.c
gcc -std=c89 -Os -o way cli.c way.o
