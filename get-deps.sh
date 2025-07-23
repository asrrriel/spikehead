#!/bin/sh

mkdir -p lib

wget -O lib/simdjson.zip https://github.com/simdjson/simdjson/releases/download/v3.13.0/singleheader.zip
mkdir -p src/core/assets/libs
unzip lib/simdjson.zip -d src/core/assets/libs

wget https://raw.githubusercontent.com/nothings/stb/f58f558c120e9b32c217290b80bad1a0729fbb2c/stb_image.h
mv stb_image.h src/core/assets/libs