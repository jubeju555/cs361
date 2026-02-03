# Project 3: Kernel Threads

**Status**: 🔲 TODO  
**Directory**: `../../project-3-kernel-threads/` (not yet created)  
**Specification**: `../../ostep-projects/concurrency-xv6-threads/`

## Overview

This project will add kernel-level threading support to xv6, building on Projects 1 and 2.

## Project Specification

Read the official specification:
```bash
cd ../../ostep-projects/concurrency-xv6-threads/
cat README.md
```

## Planned Features

To be determined after reading the project requirements.

## Prerequisites

Before starting this project:
- ✅ Complete Project 1 (Lottery Scheduling)
- 🔲 Complete Project 2 (Virtual Memory)
- 🔲 Read the project specification
- 🔲 Review xv6 process management
- 🔲 Plan implementation approach

## Getting Started

1. **Read the specification**:
   ```bash
   cd ../../ostep-projects/concurrency-xv6-threads/
   less README.md
   ```

2. **Copy Project 2 as base**:
   ```bash
   cd ../../
   cp -r project-2-virtual-memory project-3-kernel-threads
   cd project-3-kernel-threads
   ```

3. **Study xv6 process/thread code**:
   - `proc.c` - Process management
   - `swtch.S` - Context switching
   - `spinlock.c` - Synchronization

## Resources

- [xv6 Book - Chapter 5 (Scheduling)](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
- [OSTEP - Concurrency chapters](http://pages.cs.wisc.edu/~remzi/OSTEP/)
- [Project specification](../../ostep-projects/concurrency-xv6-threads/README.md)

---

**Status**: 🔲 Not started  
**Next Action**: Complete Project 2 first
