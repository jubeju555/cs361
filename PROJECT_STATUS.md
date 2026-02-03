# xv6 Operating Systems Projects

This repository contains implementations of three major xv6 operating system projects from the [OSTEP Projects](https://github.com/remzi-arpacidusseau/ostep-projects).

## Project Status

### ✅ Project 1: Lottery Scheduling (COMPLETED)
**Status**: Fully implemented and tested  
**Directory**: `/home/judah/cs361/xv6-public`  
**Documentation**: [LOTTERY_SCHEDULER.md](xv6-public/LOTTERY_SCHEDULER.md)

A proportional-share CPU scheduler that replaces xv6's round-robin scheduler with lottery scheduling. Processes are assigned tickets, and scheduling decisions are made by drawing random tickets.

**Key Features**:
- Two new system calls: `settickets()` and `getpinfo()`
- Lottery-based scheduler with O(n) ticket selection
- Ticket inheritance on fork
- Test programs: `lotterytest` and `ps`

**Quick Start**:
```bash
cd /home/judah/cs361/xv6-public
./test_lottery.sh
# Or manually:
make qemu-nox
# Then run: lotterytest
```

---

### 🔲 Project 2: Virtual Memory (TODO)
**Status**: Not started  
**Documentation**: [vm-xv6-intro](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/vm-xv6-intro)

This project will implement memory management features in xv6.

**Planned Features**:
- TBD based on project requirements

---

### 🔲 Project 3: Kernel Threads (TODO)
**Status**: Not started  
**Documentation**: [concurrency-xv6-threads](https://github.com/remzi-arpacidusseau/ostep-projects/blob/master/concurrency-xv6-threads)

This project will add kernel-level threading support to xv6.

**Planned Features**:
- TBD based on project requirements

---

## Repository Structure

```
/home/judah/cs361/
├── xv6-public/              # Main xv6 source with lottery scheduler
│   ├── pstat.h              # Process statistics structure
│   ├── proc.c               # Lottery scheduler implementation
│   ├── proc.h               # Process structure with tickets/ticks
│   ├── lotterytest.c        # Test program for lottery scheduling
│   ├── ps.c                 # Process listing utility
│   ├── LOTTERY_SCHEDULER.md # Detailed documentation
│   └── test_lottery.sh      # Quick test script
│
└── ostep-projects/          # Project specifications
    ├── scheduling-xv6-lottery/
    ├── vm-xv6-intro/
    └── concurrency-xv6-threads/
```

## Getting Started

### Prerequisites
- Linux environment (tested on Ubuntu)
- GCC with 32-bit support
- QEMU for x86
- Make

### Building xv6
```bash
cd /home/judah/cs361/xv6-public
make clean
make
make fs.img
```

### Running xv6
```bash
# Text mode (recommended)
make qemu-nox

# With graphics
make qemu
```

### Exiting QEMU
Press `Ctrl-A` then `X`

---

## Project 1: Lottery Scheduling Details

### Implementation Summary

The lottery scheduler was implemented by modifying the following xv6 components:

#### Kernel Changes
1. **New Header**: `pstat.h` - Statistics structure
2. **Process Structure**: Added `tickets` and `ticks` fields
3. **Scheduler**: Replaced round-robin with lottery algorithm
4. **System Calls**: `settickets()` and `getpinfo()`
5. **Random Number Generator**: LCG for ticket selection

#### Test Programs
- `lotterytest.c`: Validates 3:2:1 ticket ratio
- `ps.c`: Shows process statistics

### Testing

```bash
# Inside xv6
$ lotterytest
Process A (PID 4): 30 tickets
Process B (PID 5): 20 tickets  
Process C (PID 6): 10 tickets

Lottery Scheduling Results:
PID     Tickets Ticks
4       30      ~150
5       20      ~100
6       10      ~50

Expected ratio: 3:2:1 (30:20:10 tickets)
Actual ratio: ~3:2:1
```

### Key Implementation Details

**Lottery Algorithm**:
1. Sum all tickets from RUNNABLE processes
2. Generate random ticket in range [0, total)
3. Iterate processes, accumulating tickets
4. Select process where accumulated > random ticket
5. Increment tick counter and context switch

**Ticket Inheritance**:
- Default: 1 ticket per process
- Fork: Child inherits parent's tickets
- Explicit: Use `settickets(n)` to change

**Random Number Generator**:
```c
randstate = randstate * 1664525 + 1013904223;
return randstate;
```

---

## Development Timeline

- **Phase 1 (Completed)**: Lottery Scheduling
  - ✅ System call infrastructure
  - ✅ Scheduler implementation
  - ✅ Test programs
  - ✅ Documentation

- **Phase 2 (Upcoming)**: Virtual Memory
  - 🔲 Memory management features
  - 🔲 Page table operations
  - 🔲 Testing and validation

- **Phase 3 (Future)**: Kernel Threads
  - 🔲 Thread creation/management
  - 🔲 Synchronization primitives
  - 🔲 Thread scheduling

---

## Resources

- [xv6 Book](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)
- [OSTEP Book](http://pages.cs.wisc.edu/~remzi/OSTEP/)
- [OSTEP Projects Repository](https://github.com/remzi-arpacidusseau/ostep-projects)
- [Lottery Scheduling Chapter](http://www.cs.wisc.edu/~remzi/OSFEP/cpu-sched-lottery.pdf)

---

## Notes

- All work is done in the `/home/judah/cs361/` directory
- The xv6-public directory contains a working copy with lottery scheduling
- Original xv6 source is preserved in ostep-projects/initial-xv6/src/
- Each project builds incrementally on the previous one

---

## Next Steps

To continue with Project 2 (Virtual Memory):
1. Read the [vm-xv6-intro README](ostep-projects/vm-xv6-intro/README.md)
2. Understand xv6's current memory management
3. Plan the implementation approach
4. Implement and test the features

---

## Troubleshooting

### Build Errors
```bash
# Clean and rebuild
make clean
make

# Check for missing dependencies
sudo apt-get install gcc-multilib qemu-system-x86
```

### QEMU Won't Start
```bash
# Make sure QEMU is installed
which qemu-system-i386

# Try with logging
make qemu-nox QEMUEXTRA="-serial mon:stdio"
```

### Lottery Scheduler Not Working
```bash
# Check that new programs are in filesystem
ls -la _lotterytest _ps

# Rebuild filesystem
make clean
make fs.img
```

---

Last Updated: February 2, 2026  
Status: Project 1 Complete, Projects 2 & 3 Pending
