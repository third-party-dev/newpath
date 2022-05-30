#!/bin/sh

cd c89-char
./build.sh || echo "FAILURE: c89-char/build.sh"
cd ..

