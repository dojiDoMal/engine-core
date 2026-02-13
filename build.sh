#!/bin/sh

# Defaults
TARGET="native"
CLEAN=0
RUN=0

# -------------------------
# Parse arguments
# -------------------------
while [ $# -gt 0 ]; do
    case "$1" in
        -c|--clean)
            CLEAN=1
            shift
            ;;
        -r|--run)
            RUN=1
            shift
            ;;
        -t|--target)
            TARGET="$2"
            shift 2
            ;;
        *)
            shift
            ;;
    esac
done

# -------------------------
# Build dir
# -------------------------
if [ "$TARGET" = "web" ]; then
    BUILD_DIR="build/web"
else
    BUILD_DIR="build/pc"
fi

# -------------------------
# Clean
# -------------------------
if [ "$CLEAN" -eq 1 ]; then
    echo "Cleaning $BUILD_DIR..."
    rm -rf "$BUILD_DIR"
fi

# -------------------------
# Web build
# -------------------------
if [ "$TARGET" = "web" ]; then
    echo "Building for WebGL with CMake preset..."
    emcmake cmake --preset web || exit 1
    cmake --build "$BUILD_DIR" || exit 1

    if [ "$RUN" -eq 1 ]; then
        echo "Running Web build on http://localhost:8000"
        cd "$BUILD_DIR" || exit 1
        python3 -m http.server 8000
    else
        echo "Build completed."
        echo "To run: python3 -m http.server 8000 -d $BUILD_DIR"
    fi

# -------------------------
# Native build
# -------------------------
else
    echo "Building for native..."

    # Configure only if CMakeCache.txt does not exist
    if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
        cmake -B "$BUILD_DIR"
        if [ $? -ne 0 ]; then
            echo "Build configuration failed!"
            exit 1
        fi
    fi

    cmake --build "$BUILD_DIR"
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi

    if [ "$RUN" -eq 1 ]; then
        echo "Running native build..."
        "$BUILD_DIR/main"
    else
        echo "Build completed."
    fi
fi
