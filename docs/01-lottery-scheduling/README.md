# Project 1: Lottery Scheduling

**Status**: ✅ COMPLETED  
**Directory**: `../../project-1-lottery-scheduler/`  
**Duration**: February 2026

## Overview

This project implements a **lottery scheduler** for xv6, replacing the default round-robin scheduler with a proportional-share scheduling algorithm. Processes are assigned "tickets," and scheduling decisions are made by holding a lottery—processes with more tickets have a higher probability of winning CPU time.

## Quick Links

- **[Code Walkthrough](code-walkthrough.md)** - 🎓 Step-by-step code explanation (START HERE)
- **[Implementation Details](implementation.md)** - Full technical documentation
- **[Quick Reference](quick-reference.md)** - Testing and usage guide
- **[Source Code](../../project-1-lottery-scheduler/)** - Working xv6 implementation

## What Was Implemented

### 1. Lottery Scheduler
Replaced xv6's round-robin scheduler with lottery scheduling:
- Random ticket selection using LCG algorithm
- O(n) scheduling with proportional fairness
- Tick counter to track process execution

### 2. System Calls

#### `int settickets(int number)`
Sets the number of lottery tickets for the calling process.
- Returns `0` on success, `-1` if `number < 1`
- Default: 1 ticket per process

#### `int getpinfo(struct pstat *ps)`
Retrieves statistics about all running processes.
- Returns `0` on success, `-1` on error
- Fills `pstat` structure with process info

### 3. Test Programs

#### `lotterytest`
Comprehensive test that:
- Creates 3 processes with 30:20:10 ticket ratio
- Performs CPU-intensive work
- Reports actual vs. expected tick distribution

#### `ps`
Simple process viewer that displays:
- Process ID (PID)
- Number of tickets
- Total ticks (times scheduled)

## Results

The implementation successfully demonstrates proportional-share scheduling:

```
Expected Ratio: 3:2:1 (30:20:10 tickets)
Actual Results: ~3:2:1 with acceptable variance
```

Processes with more tickets consistently receive more CPU time over the long run.

## Key Concepts Learned

1. **CPU Scheduling Algorithms**: Understanding how schedulers make decisions
2. **Randomness in OS**: Using pseudo-random numbers for fairness
3. **System Calls**: Adding new kernel-user interfaces
4. **Process Management**: Modifying core process structures
5. **Testing**: Validating probabilistic systems

## File Changes

### New Files
- `pstat.h` - Process statistics structure
- `lotterytest.c` - Test program
- `ps.c` - Process viewer
- `test_lottery.sh` - Automated test script

### Modified Files
- `proc.h` - Added tickets/ticks to process structure
- `proc.c` - Lottery scheduler implementation
- `defs.h` - Function declarations
- `syscall.h`, `syscall.c` - System call registration
- `sysproc.c` - System call implementations
- `usys.S`, `user.h` - User-space interface
- `Makefile` - Build configuration

## Testing

### Quick Test
```bash
cd ../../project-1-lottery-scheduler/
./test_lottery.sh
```

### Manual Testing
```bash
cd ../../project-1-lottery-scheduler/
make qemu-nox

# Inside xv6:
$ lotterytest
$ ps
```

### Exit QEMU
Press `Ctrl-A` then `X`

## Performance Notes

- **Complexity**: O(n) per scheduling decision
- **Overhead**: Minimal - simple random number generation
- **Fairness**: Converges to expected ratios over time
- **Variance**: Natural due to probabilistic selection

## Common Issues Encountered

### Issue 1: Random Number Generation
**Problem**: Need randomness in kernel  
**Solution**: Implemented LCG algorithm

### Issue 2: System Call Arguments
**Problem**: Passing pointers from user space  
**Solution**: Used `argptr()` with safety checks

### Issue 3: Tick Counting
**Problem**: When to increment ticks  
**Solution**: Increment when process is selected by scheduler

## Next Steps

With lottery scheduling complete, the next project is:

**Project 2: Virtual Memory**
- Location: `../../ostep-projects/vm-xv6-intro/`
- Documentation: `../02-virtual-memory/`

## References

- [OSTEP Lottery Scheduling](http://www.cs.wisc.edu/~remzi/OSFEP/cpu-sched-lottery.pdf)
- [Project Specification](../../ostep-projects/scheduling-xv6-lottery/README.md)
- [xv6 Book - Chapter 5 (Scheduling)](https://pdos.csail.mit.edu/6.828/2018/xv6/book-rev11.pdf)

---

**Status**: ✅ Complete and tested  
**Started**: February 2, 2026  
**Completed**: February 2, 2026
