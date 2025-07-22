#!/bin/sh

set -x

PLATFORM=linux

mkdir -p bin/$PLATFORM

SRC_FILES=$(find src/core -name '*.cpp' -print)
RENDERER_FILES=$(find src/renderer -name '*.cpp' -print)
PLATFORM_FILES=$(find src/platform/$PLATFORM -name '*.cpp' -print)

ALL_CPP="$SRC_FILES $PLATFORM_FILES $RENDERER_FILES"

CFLAGS="-Isrc/core -Isrc/platform/$PLATFORM -Isrc/misc -Isrc/renderer -DPLATFORM=\"$PLATFORM\""
LDFLAGS="-lGL -lX11"

CLANG_CMD="clang++ -std=c++20 -O2 $INCLUDE_XCB $CFLAGS -c"

ESCAPED_CLANG_CMD=$(printf '%s' "$CLANG_CMD" | sed 's/"/\\"/g')

# Build compile_commands.json
echo "[" > compile_commands.json
SEP=""
for SRC in $ALL_CPP; do
    echo "$SEP{" >> compile_commands.json
    echo "  \"directory\": \"$(pwd)\"," >> compile_commands.json
    echo "  \"command\": \"$ESCAPED_CLANG_CMD $SRC\"," >> compile_commands.json
    echo "  \"file\": \"$SRC\"" >> compile_commands.json
    echo -n "}" >> compile_commands.json
    SEP=","
done
echo "]" >> compile_commands.json

clang++ -std=c++20 -O2 \
  $CFLAGS \
  $ALL_CPP \
  $LDFLAGS \
  -o bin/$PLATFORM/release
