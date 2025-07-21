#!/bin/sh

PLATFORM=linux-gl  # or whatever platform you want

mkdir -p bin/$PLATFORM

SRC_FILES=$(find src/core -name '*.cpp' -print)
PLATFORM_FILES=$(find src/platform/$PLATFORM -name '*.cpp' -print)

ALL_CPP="$SRC_FILES $PLATFORM_FILES"

INCLUDE_XCB="-Ilib/libxcb/build/include -Ilib/xcb-proto/build/include"
LIB_XCB="-Llib/libxcb/build/lib -lxcb -lXau -lXdmcp"

clang++ -std=c++20 -O2 \
  $INCLUDE_XCB \
  $ALL_CPP \
  $LIB_XCB \
  -o bin/$PLATFORM/release