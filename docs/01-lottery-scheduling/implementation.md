# Lottery Scheduling Implementation for xv6

This project implements a lottery scheduler for xv6, replacing the original round-robin scheduler with a proportional-share scheduling algorithm.

## Overview

Lottery scheduling is a probabilistic scheduling algorithm where each process is assigned "tickets" representing its share of the CPU. At each scheduling decision, a random ticket is drawn, and the process holding that ticket runs for the next time slice. Processes with more tickets have a proportionally higher chance of being selected.

## Implementation Details

### Core Changes

1. **Data Structures** ([pstat.h](pstat.h))
   - Added `pstat.h` header defining the `pstat` structure to track process statistics
   - Fields: `inuse[]`, `tickets[]`, `pid[]`, `ticks[]`

2. **Process Structure** ([proc.h](proc.h))
   - Added `tickets` field to track each process's ticket count
   - Added `ticks` field to count how many times a process has been scheduled

3. **Random Number Generator** ([proc.c](proc.c))
   - Implemented a simple linear congruential generator (LCG) for lottery ticket selection
   - Function: `random()`

4. **System Calls**
   - **`settickets(int number)`**: Sets the number of tickets for the calling process
     - Returns 0 on success, -1 if tickets < 1
     - Modified files: [syscall.h](syscall.h), [syscall.c](syscall.c), [sysproc.c](sysproc.c), [usys.S](usys.S), [user.h](user.h), [defs.h](defs.h)
   
   - **`getpinfo(struct pstat *)`**: Returns information about all running processes
     - Returns 0 on success, -1 on error
     - Modified files: Same as above + [proc.c](proc.c) (implementation)

5. **Scheduler** ([proc.c](proc.c))
   - Replaced round-robin scheduler with lottery scheduling in `scheduler()` function
   - Algorithm:
     1. Count total tickets of all RUNNABLE processes
     2. Pick a random winning ticket
     3. Iterate through processes, summing tickets until we exceed the winning ticket
     4. Run the winning process and increment its tick count

6. **Process Creation**
   - **`allocproc()`**: Initialize new processes with 1 ticket by default
   - **`fork()`**: Child processes inherit ticket count from parent

### Test Programs

1. **[lotterytest.c](lotterytest.c)**
   - Creates 3 processes with a 3:2:1 ticket ratio (30:20:10 tickets)
   - Each process does compute-intensive work
   - Reports the actual tick distribution vs. expected ratio
   - Use to verify lottery scheduling works correctly

2. **[ps.c](ps.c)**
   - Simple ps-like utility to show all running processes
   - Displays PID, tickets, and tick count for each process
   - Useful for monitoring lottery scheduling in real-time

## Building and Running

### Build xv6 with Lottery Scheduling
```bash
cd /home/judah/cs361/xv6-public
make clean
make
make fs.img
```

### Run in QEMU
```bash
make qemu-nox  # Text mode
# or
make qemu      # With graphics
```

### Test the Implementation
Inside xv6:
```bash
# Run the lottery test
lotterytest

# View process statistics
ps
```

## Expected Results

When running `lotterytest`, you should see output similar to:
```
Process A (PID 4): 30 tickets
Process B (PID 5): 20 tickets
Process C (PID 6): 10 tickets

Lottery Scheduling Results:
PID     Tickets Ticks
4       30      150
5       20      100
6       10      50

Expected ratio: 3:2:1 (30:20:10 tickets)
Actual ratio: 3:2:1
```

The actual ratio should approximate 3:2:1, though some variance is expected due to the probabilistic nature of lottery scheduling.

## Files Modified

### Kernel Files
- [pstat.h](pstat.h) - New file
- [proc.h](proc.h) - Added tickets and ticks fields
- [proc.c](proc.c) - Lottery scheduler, random number generator, getpinfo()
- [defs.h](defs.h) - Added getpinfo() declaration
- [syscall.h](syscall.h) - Added system call numbers
- [syscall.c](syscall.c) - Registered new system calls
- [sysproc.c](sysproc.c) - System call implementations
- [usys.S](usys.S) - User-space system call stubs
- [user.h](user.h) - User-space function declarations

### User Programs
- [lotterytest.c](lotterytest.c) - Test program for lottery scheduling
- [ps.c](ps.c) - Process listing utility

### Build System
- [Makefile](Makefile) - Added new test programs to UPROGS

## Technical Notes

### Random Number Generator
The implementation uses a linear congruential generator (LCG) with the formula:
```
next = current * 1664525 + 1013904223
```
This is a simple but effective pseudo-random number generator suitable for kernel use.

### Ticket Inheritance
When a process forks, the child inherits the parent's ticket count. This ensures that processes maintain their scheduling priority across forks.

### Default Tickets
All processes start with 1 ticket by default (set in `allocproc()`). This ensures fairness when no explicit ticket allocation is made.

### Scheduler Algorithm
The lottery scheduler:
1. Holds the process table lock
2. Counts total tickets of RUNNABLE processes
3. Picks a random ticket number between 0 and (total_tickets - 1)
4. Iterates through processes, accumulating ticket counts
5. Selects the first process where accumulated tickets > winning ticket
6. Increments the selected process's tick counter
7. Context switches to the selected process

This O(n) algorithm is simple and effective for the small number of processes typically running in xv6.

## Next Steps

After completing lottery scheduling, the next projects are:
1. **Virtual Memory** - Implementing memory management features
2. **Kernel Threads** - Adding thread support to xv6

## References

- [OSTEP Lottery Scheduling Chapter](http://www.cs.wisc.edu/~remzi/OSFEP/cpu-sched-lottery.pdf)
- [Project Repository](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/scheduling-xv6-lottery)
