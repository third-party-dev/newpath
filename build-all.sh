#!/bin/sh

cd c89-char
./build.sh || echo "FAILURE: c89-char/build.sh"
cd ..
PREFIX=c89-char ./test/test-cli.sh
PREFIX=c89-char ./test/test-cli-utf8.sh
