#!/bin/sh

docker run -ti --rm -v $(pwd):/workspace -w /workspace node "$@"