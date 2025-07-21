#!/bin/sh
set -e

BASE_DIR=$(pwd)/lib
MUSL_DIR=$BASE_DIR/musl
X11_DIR=$BASE_DIR/x11

mkdir -p $BASE_DIR

# --------------------------
# Build musl
# --------------------------
cd $BASE_DIR
if [ ! -d musl ]; then
    git clone git://git.musl-libc.org/musl
fi

cd musl
./configure --prefix=$MUSL_DIR/build --disable-shared
make -j$(nproc)
make install

# --------------------------
# Build XCB 
# --------------------------
cd $BASE_DIR
if [ ! -d $BASE_DIR/libxcb ]; then
  git clone https://gitlab.freedesktop.org/xorg/lib/libxcb.git $BASE_DIR/libxcb
fi
cd $BASE_DIR/libxcb
./autogen.sh --prefix=$BASE_DIR/libxcb/build --disable-shared --enable-static
make -j$(nproc)
make install

cd $BASE_DIR
if [ ! -d $BASE_DIR/xcb-proto ]; then
  git clone https://gitlab.freedesktop.org/xorg/proto/xcbproto.git $BASE_DIR/xcb-proto
fi
cd $BASE_DIR/xcb-proto
./autogen.sh --prefix=$BASE_DIR/xcb-proto/build --disable-shared --enable-static
make -j$(nproc)
make install