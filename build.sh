#!/bin/bash

# KaliShell Build Script

set -e

echo "Building KaliShell..."

# Check for required tools
command -v gcc >/dev/null 2>&1 || { echo "gcc not found. Installing..."; sudo apt-get install -y gcc; }
command -v make >/dev/null 2>&1 || { echo "make not found. Installing..."; sudo apt-get install -y make; }
command -v pkg-config >/dev/null 2>&1 || { echo "pkg-config not found. Installing..."; sudo apt-get install -y pkg-config; }

# Check for development libraries
echo "Checking for required libraries..."

# SQLite3
if ! pkg-config --exists sqlite3 2>/dev/null; then
    echo "SQLite3 development libraries not found. Installing..."
    sudo apt-get install -y libsqlite3-dev
fi

# ncurses
if ! pkg-config --exists ncurses 2>/dev/null; then
    echo "ncurses development libraries not found. Installing..."
    sudo apt-get install -y libncurses-dev
fi

# Lua (optional)
if ! pkg-config --exists lua5.4 2>/dev/null; then
    echo "Lua 5.4 not found. Installing..."
    sudo apt-get install -y liblua5.4-dev || echo "Lua not available, continuing without Lua support"
fi

# Create build directory
mkdir -p build

# Build with make
echo "Compiling..."
make -j$(nproc)

echo ""
echo "Build complete!"
echo "Run with: ./build/kalishell"
echo ""
