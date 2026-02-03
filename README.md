# CS361 - Operating Systems Projects

Three xv6-based projects implementing core OS concepts: scheduling, virtual memory, and kernel threads.

## 📁 Project Structure

```
cs361/
├── README.md                          ← You are here
├── docs/                              ← All documentation
│   ├── 01-lottery-scheduling/        ← Project 1 docs
│   ├── 02-virtual-memory/            ← Project 2 docs
│   └── 03-kernel-threads/            ← Project 3 docs
├── project-1-lottery-scheduler/      ← ✅ COMPLETED
│   └── [xv6 source with lottery scheduling]
├── project-2-virtual-memory/         ← 🔲 TODO (future)
├── project-3-kernel-threads/         ← 🔲 TODO (future)
└── ostep-projects/                   ← Reference materials
    └── [project specifications]
```

## 🎯 Current Status

### Project 1: Lottery Scheduling ✅ COMPLETED
**Directory**: `project-1-lottery-scheduler/`  
**Status**: Fully implemented and tested  
**Documentation**: [docs/01-lottery-scheduling/](docs/01-lottery-scheduling/)

Implements proportional-share CPU scheduling using lottery tickets. Processes with more tickets get more CPU time.

**Quick Start**:
```bash
cd project-1-lottery-scheduler
make qemu-nox
# Inside xv6:
$ lotterytest
$ ps
```

### Project 2: Virtual Memory 🔲 TODO
**Directory**: `project-2-virtual-memory/` (not yet created)  
**Specification**: `ostep-projects/vm-xv6-intro/`  
**Documentation**: [docs/02-virtual-memory/](docs/02-virtual-memory/)

Will implement memory management features in xv6.

### Project 3: Kernel Threads 🔲 TODO
**Directory**: `project-3-kernel-threads/` (not yet created)  
**Specification**: `ostep-projects/concurrency-xv6-threads/`  
**Documentation**: [docs/03-kernel-threads/](docs/03-kernel-threads/)

Will add kernel-level threading support to xv6.

---

## 📚 Documentation

All documentation is organized in the `docs/` directory:

- **[01-lottery-scheduling/](docs/01-lottery-scheduling/)** - Complete docs for Project 1
  - [README.md](docs/01-lottery-scheduling/README.md) - Overview
  - [implementation.md](docs/01-lottery-scheduling/implementation.md) - Implementation details
  - [quick-reference.md](docs/01-lottery-scheduling/quick-reference.md) - Testing guide

- **[02-virtual-memory/](docs/02-virtual-memory/)** - Docs for Project 2 (TBD)
- **[03-kernel-threads/](docs/03-kernel-threads/)** - Docs for Project 3 (TBD)

---

## 🚀 Quick Navigation

### Working on Project 1 (Lottery Scheduling)
```bash
cd project-1-lottery-scheduler/
./test_lottery.sh              # Run tests
make qemu-nox                  # Boot xv6
```

### Reading Documentation
```bash
cd docs/01-lottery-scheduling/
cat README.md                  # Project overview
cat quick-reference.md         # Testing guide
```

### Starting Next Project
```bash
cd ostep-projects/vm-xv6-intro/
cat README.md                  # Read requirements
```

---

## 🛠️ Prerequisites

- **OS**: Linux (tested on Ubuntu)
- **Compiler**: GCC with 32-bit support
- **Emulator**: QEMU for x86
- **Build Tool**: Make

### Install Dependencies
```bash
sudo apt-get update
sudo apt-get install gcc-multilib qemu-system-x86 make
```

---

## 📖 Learning Resources

- [xv6 Book](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf) - Complete xv6 guide
- [OSTEP Book](http://pages.cs.wisc.edu/~remzi/OSTEP/) - Operating Systems concepts
- [Project Repository](https://github.com/remzi-arpacidusseau/ostep-projects) - Official project specs

---

## 🗺️ Project Roadmap

- [x] **Phase 1**: Lottery Scheduling (COMPLETED)
  - [x] Implement lottery scheduler
  - [x] Add `settickets()` and `getpinfo()` system calls
  - [x] Create test programs
  - [x] Documentation

- [ ] **Phase 2**: Virtual Memory (TODO)
  - [ ] Read project requirements
  - [ ] Plan implementation
  - [ ] Implement features
  - [ ] Test and document

- [ ] **Phase 3**: Kernel Threads (TODO)
  - [ ] Read project requirements
  - [ ] Plan implementation
  - [ ] Implement features
  - [ ] Test and document

---

## 💡 Tips

- **Always read docs first**: Check `docs/` before diving into code
- **Test incrementally**: Build and test after each feature
- **Keep notes**: Document issues and solutions as you go
- **Backup work**: Commit changes regularly

---

## 🆘 Help

### Build Issues
```bash
cd project-1-lottery-scheduler/
make clean && make
```

### Documentation
All guides are in `docs/` - start with the README for each project.

### QEMU Controls
- **Exit QEMU**: Press `Ctrl-A` then `X`
- **Switch views**: `Ctrl-A` then `C` (console/monitor)

---

**Current Project**: Lottery Scheduling (✅ Complete)  
**Next Up**: Virtual Memory (🔲 Pending)  
**Last Updated**: February 2, 2026
