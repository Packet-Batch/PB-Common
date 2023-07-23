#!/bin/bash
THREADS=1

# Check for core numbers.
if [ -n "$1" ]; then
    if [ "$1" -eq 0 ]; then
        THREADS=$(nproc)
    elif [ "$1" -gt 0 ]; then
        THREADS=$1
    fi
fi

echo "Building Packet Batch (Common) using $THREADS threads..."

# First, we want to build LibYAML.
echo "Building LibYAML..."
sudo make -j $THREADS libyaml
echo "Done..."

# Next build main project.
echo "Building Main..."
make -j $THREADS
echo "Done..."

# Finally, install common files.
echo "Installing Main..."
sudo make -j $THREADS install
echo "Done..."