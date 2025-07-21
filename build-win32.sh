#!/bin/sh

set -x

PLATFORM=win32

mkdir -p bin/$PLATFORM

SRC_FILES=$(find src/core -name '*.cpp' -print)
PLATFORM_FILES=$(find src/platform/$PLATFORM -name '*.cpp' -print)

ALL_CPP="$SRC_FILES $PLATFORM_FILES"

CFLAGS="-Isrc/core -Isrc/platform/$PLATFORM -Isrc/core/platform_glue -DPLATFORM=\"$PLATFORM\""
LDFLAGS="-static -luser32 -lgdi32 -lkernel32 -lshell32 -lpthread"

TARGET="x86_64-w64-mingw32"
CLANG_CMD="clang++ --target=$TARGET -std=c++20 -O2 $CFLAGS -c"

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

clang++ --target=$TARGET -std=c++20 -O2 \
  $CFLAGS \
  $ALL_CPP \
  $LDFLAGS \
  -o bin/$PLATFORM/release.exe
