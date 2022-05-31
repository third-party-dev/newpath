#!/bin/sh

# Create user binary folder.
mkdir -p ~/.local/bin

# Build way if the binary doesn't exist.
[ -e way ] || ./build.sh

# Copy binary to user binary folder.
[ -e ~/.local/bin/way ] || cp way ~/.local/bin/way

# TODO: Notify user if user bianry folder not in path?
