# 🗺️ Navigation Cheat Sheet

Quick reference for navigating the reorganized CS361 project structure.

## 📂 Where Am I?

```bash
pwd  # Shows current directory
```

### Main Locations

| Directory | Purpose | Command |
|-----------|---------|---------|
| **Root** | Project home | `cd ~/cs361` |
| **Docs** | All documentation | `cd ~/cs361/docs` |
| **Project 1** | Lottery scheduler code | `cd ~/cs361/project-1-lottery-scheduler` |
| **Specs** | Project requirements | `cd ~/cs361/ostep-projects` |

## 🚀 Common Tasks

### Read Documentation
```bash
cd ~/cs361
cat README.md                                    # Start here!
cat docs/01-lottery-scheduling/README.md         # Project 1 overview
cat docs/01-lottery-scheduling/quick-reference.md # Testing guide
```

### Work on Project 1
```bash
cd ~/cs361/project-1-lottery-scheduler
./test_lottery.sh        # Quick test
make qemu-nox           # Run xv6
```

### Check Project Specifications
```bash
cd ~/cs361/ostep-projects
ls -la                                          # See all projects
cat scheduling-xv6-lottery/README.md            # Project 1 spec
cat vm-xv6-intro/README.md                      # Project 2 spec
cat concurrency-xv6-threads/README.md           # Project 3 spec
```

### Build and Test
```bash
# From project directory
cd ~/cs361/project-1-lottery-scheduler
make clean              # Clean build artifacts
make                    # Build kernel
make fs.img             # Build filesystem
make qemu-nox           # Test in QEMU
```

## 📁 Complete Directory Map

```
~/cs361/
│
├── README.md                          ← START HERE - Main entry point
├── NAVIGATION.md                      ← This file
│
├── docs/                              ← 📚 All Documentation
│   ├── 01-lottery-scheduling/        
│   │   ├── README.md                  ← Project 1 overview
│   │   ├── implementation.md          ← Technical details
│   │   └── quick-reference.md         ← Testing guide
│   ├── 02-virtual-memory/
│   │   └── README.md                  ← Project 2 (future)
│   └── 03-kernel-threads/
│       └── README.md                  ← Project 3 (future)
│
├── project-1-lottery-scheduler/       ← ✅ ACTIVE PROJECT
│   ├── [xv6 source files]
│   ├── test_lottery.sh                ← Quick test script
│   ├── lotterytest.c                  ← Test program
│   └── ps.c                           ← Process viewer
│
├── project-2-virtual-memory/          ← 🔲 Future
├── project-3-kernel-threads/          ← 🔲 Future
│
└── ostep-projects/                    ← 📖 Reference specs
    ├── scheduling-xv6-lottery/        ← Project 1 spec
    ├── vm-xv6-intro/                  ← Project 2 spec
    └── concurrency-xv6-threads/       ← Project 3 spec
```

## 🎯 Quick Commands by Task

### Just Want to See It Work?
```bash
cd ~/cs361/project-1-lottery-scheduler && ./test_lottery.sh
```

### Want to Understand It?
```bash
cd ~/cs361/docs/01-lottery-scheduling && cat README.md
```

### Want to Modify Code?
```bash
cd ~/cs361/project-1-lottery-scheduler
# Edit files, then:
make clean && make && make fs.img
make qemu-nox
```

### Starting Project 2?
```bash
cd ~/cs361
cat docs/02-virtual-memory/README.md           # Read overview
cat ostep-projects/vm-xv6-intro/README.md      # Read spec
# Then copy Project 1 as base:
cp -r project-1-lottery-scheduler project-2-virtual-memory
cd project-2-virtual-memory
```

## 🔍 Finding Things

### Find a File
```bash
cd ~/cs361
find . -name "*.c" | grep lottery    # Find C files with "lottery"
find . -name "*.md"                  # Find all markdown docs
```

### Search Documentation
```bash
cd ~/cs361/docs
grep -r "settickets" .               # Search for "settickets"
grep -r "system call" .              # Search for "system call"
```

### List Test Programs
```bash
cd ~/cs361/project-1-lottery-scheduler
ls -la _*                            # List all compiled programs
```

## 📊 Project Status at a Glance

```bash
cd ~/cs361
ls -1d project-* 2>/dev/null         # See which projects exist
ls -1 docs/*/README.md               # See available docs
```

## 🆘 Help

### Lost? Start Fresh
```bash
cd ~/cs361
cat README.md                        # Main guide
ls -la                               # See what's here
```

### Can't Find Documentation?
```bash
cd ~/cs361/docs
find . -name "*.md"                  # List all docs
```

### Build Broken?
```bash
cd ~/cs361/project-1-lottery-scheduler
make clean && make
```

## 💡 Pro Tips

1. **Bookmark these paths**:
   ```bash
   echo "alias cs361='cd ~/cs361'" >> ~/.bashrc
   echo "alias p1='cd ~/cs361/project-1-lottery-scheduler'" >> ~/.bashrc
   echo "alias docs='cd ~/cs361/docs'" >> ~/.bashrc
   source ~/.bashrc
   
   # Then use:
   cs361    # Go to root
   p1       # Go to Project 1
   docs     # Go to documentation
   ```

2. **Quick status check**:
   ```bash
   cd ~/cs361 && head -20 README.md
   ```

3. **Tab completion is your friend**:
   ```bash
   cd ~/cs361/proj<TAB>      # Auto-completes
   cd ~/cs361/docs/01<TAB>   # Auto-completes
   ```

---

**Tip**: Keep this file open in a terminal while working!

```bash
cd ~/cs361 && less NAVIGATION.md
```
