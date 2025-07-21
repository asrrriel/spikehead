#!/bin/sh

set -x

PLATFORM=linux-gl  # or whatever platform you want

mkdir -p bin/$PLATFORM

SRC_FILES=$(find src/core -name '*.cpp' -print)
PLATFORM_FILES=$(find src/platform/$PLATFORM -name '*.cpp' -print)

ALL_CPP="$SRC_FILES $PLATFORM_FILES"

INCLUDE_XCB="-Ilib/libxcb/build/include -Ilib/xcb-proto/build/include"
LIB_XCB="-Llib/libxcb/build/lib -lxcb -lXau -lXdmcp"

CFLAGS="-Isrc/core -Isrc/platform/$PLATFORM -Isrc/core/platform_glue -DPLATFORM=\"$PLATFORM\""
LDFLAGS=""

CLANG_CMD="clang++ -std=c++20 -O2 $INCLUDE_XCB $CFLAGS -c"

# Build compile_commands.json
echo "[" > compile_commands.json
SEP=""
for SRC in $ALL_CPP; do
    echo "$SEP{" >> compile_commands.json
    echo "  \"directory\": \"$(pwd)\"," >> compile_commands.json
    echo "  \"command\": \"$CLANG_CMD $SRC\"," >> compile_commands.json
    echo "  \"file\": \"$SRC\"" >> compile_commands.json
    echo -n "}" >> compile_commands.json
    SEP=","
done
echo "]" >> compile_commands.json

clang++ -std=c++20 -O2 \
  $INCLUDE_XCB \
  $CFLAGS \
  $ALL_CPP \
  $LIB_XCB \
  $LDFLAGS \
  -o bin/$PLATFORM/release
