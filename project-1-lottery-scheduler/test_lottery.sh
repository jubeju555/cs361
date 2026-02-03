#!/bin/bash
# Quick test script for lottery scheduling

cd /home/judah/cs361/project-1-lottery-scheduler

echo "Building xv6 with lottery scheduling..."
make clean > /dev/null 2>&1
make > /dev/null 2>&1
make fs.img > /dev/null 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "To test the lottery scheduler:"
    echo "1. Run: make qemu-nox"
    echo "2. At the xv6 prompt, type: lotterytest"
    echo "3. Or type: ps (to see process info)"
    echo "4. Press Ctrl-A then X to exit QEMU"
    echo ""
    echo "Starting xv6 now..."
    echo "Press Ctrl-A then X to exit"
    sleep 2
    make qemu-nox
else
    echo "Build failed!"
    exit 1
fi
