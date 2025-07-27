#!/bin/sh

# ========== command line input =============
ACTION="$1"
PLATFORM="$2"

# ====== make sure directories exist ========
mkdir -p tmp
mkdir -p obj
mkdir -p bin
mkdir -p bin/$PLATFORM

# ================ checks ===================
[ -z "$ACTION" ] && echo "Usage: $0 <action>" && exit 1

if [ ! -d "src/platform/$PLATFORM" ]; then
    echo "Unknown/unsupported platform: $PLATFORM"
    exit 1
fi

# =========== variable defintions ===========
CORE_FILES=$(find src/core -name '*.cpp')
LIB_FILES=$(find src/lib -name '*.cpp')
PLATFORM_FILES=$(find src/platform/$PLATFORM -name '*.cpp')
ALL_CPP="$CORE_FILES $PLATFORM_FILES $LIB_FILES"

LDFLAGS=""
CFLAGS="-std=c++20 -O2 -Isrc/include -Isrc/lib -DPLATFORM=\"$PLATFORM\" -DVERSION=\"unreleased\""

if [ "$PLATFORM" = "win32" ]; then
    TARGET="x86_64-w64-mingw32"
    CFLAGS="$CFLAGS"
    LDFLAGS="-lopengl32 -luser32 -lgdi32 -lkernel32 -lshell32 -lpthread"
    CLANG_CMD="clang++ $CFLAGS -c"
    OUTFILE="bin/$PLATFORM/release.exe"
elif [ "$PLATFORM" = "linux" ]; then
    TARGET="x86_64-linux-gnu"
    LDFLAGS="-lGL -lX11"
    CLANG_CMD="clang++ $CFLAGS -c"
    OUTFILE="bin/$PLATFORM/release"
fi

# ========== non-building actions ==========
if [ "$ACTION" = "clean" ]; then
    rm -rf tmp src/lib/simdjson src/lib/nothings bin obj compile_commands.json
    find . -name '*.o' -type f -delete
    exit 0
fi

# ================ dependencies ============
if [ ! -d "src/lib/simdjson" ]; then
    wget -O tmp/simdjson.zip https://github.com/simdjson/simdjson/releases/download/v3.13.0/singleheader.zip
    mkdir -p src/lib/simdjson
    unzip tmp/simdjson.zip -d src/lib/simdjson
fi

if [ ! -d "src/lib/nothings" ]; then
    mkdir -p src/lib/nothings
    wget -O src/lib/nothings/stb_image.h https://raw.githubusercontent.com/nothings/stb/f58f558c120e9b32c217290b80bad1a0729fbb2c/stb_image.h
fi

if [ ! -f "bin/win32/libwinpthread-1.dll" ] && [ "$PLATFORM" = "win32" ]; then
    cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll bin/win32/
fi


# ============ building actions ============
if [ "$ACTION" = "build" ] || [ "$ACTION" = "run" ]; then
    [ -z "$PLATFORM" ] && echo "Usage: $0 build/run <platform>" && exit 1
    
    ESCAPED_CLANG_CMD=$(printf '%s' "$CLANG_CMD" | sed 's/"/\\"/g')

    if [ -f "$OUTFILE" ]; then
        OUTFILE_LAST_MODIFIED=$(stat -c %Y "$OUTFILE")
    else 
        OUTFILE_LAST_MODIFIED=0
    fi

    O_FILES=""
    
    CMD_FILE=compile_commands.json

    echo "[" > "$CMD_FILE"
    SEP=""
    for SRC in $ALL_CPP; do
        echo "$SEP{" >> "$CMD_FILE"
        echo "  \"directory\": \"$(pwd)\"," >> "$CMD_FILE"
        echo "  \"command\": \"$ESCAPED_CLANG_CMD $SRC\"," >> "$CMD_FILE"
        echo "  \"file\": \"$SRC\"" >> "$CMD_FILE"
        echo -n "}" >> "$CMD_FILE"
        SEP=","

        OBJ=$(echo "$SRC" | sed 's|src/|obj/|' | sed 's/\.cpp$/.o/')
        O_FILES="$O_FILES $OBJ"

        if [ $(stat -c %Y "$SRC") -gt "$OUTFILE_LAST_MODIFIED" ]; then
            echo "Compiling $SRC..."
            mkdir -p "$(dirname $OBJ)"
            $CLANG_CMD "--target=$TARGET" "$SRC" -o "$OBJ"
        fi
    done
    echo "]" >> "$CMD_FILE"

    echo "Building $OUTFILE..."
    clang++ "--target=$TARGET" $O_FILES $LDFLAGS -o "$OUTFILE"
fi

if [ "$ACTION" = "run" ]; then
    echo "Running $OUTFILE..."
    if [ "$PLATFORM" = "win32" ]; then
        wine "$OUTFILE"
    else
        "$OUTFILE"
    fi
fi