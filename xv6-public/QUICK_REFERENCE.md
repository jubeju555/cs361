# Lottery Scheduling - Quick Reference

## Testing the Implementation

### Method 1: Using the Test Script
```bash
cd /home/judah/cs361/xv6-public
./test_lottery.sh
```

### Method 2: Manual Testing
```bash
cd /home/judah/cs361/xv6-public
make qemu-nox
# Inside xv6:
$ lotterytest
$ ps
```

### Exiting QEMU
Press `Ctrl-A` then `X`

---

## System Calls Added

### settickets(int number)
Sets the number of lottery tickets for the calling process.

```c
int tickets = 30;
if (settickets(tickets) == 0) {
    // Success
} else {
    // Error (tickets < 1)
}
```

### getpinfo(struct pstat *ps)
Retrieves information about all processes.

```c
#include "pstat.h"

struct pstat ps;
if (getpinfo(&ps) == 0) {
    for (int i = 0; i < NPROC; i++) {
        if (ps.inuse[i]) {
            printf(1, "PID: %d, Tickets: %d, Ticks: %d\n",
                   ps.pid[i], ps.tickets[i], ps.ticks[i]);
        }
    }
}
```

---

## Test Programs

### lotterytest
Tests lottery scheduling with a 3:2:1 ticket ratio.

**Usage**:
```bash
$ lotterytest
```

**Expected Output**:
```
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

### ps
Shows process information including tickets and ticks.

**Usage**:
```bash
$ ps
```

**Output**:
```
PID     Tickets Ticks
1       1       45
2       1       23
3       30      156
4       20      104
5       10      52
```

---

## How Lottery Scheduling Works

### Algorithm
1. Count total tickets from all RUNNABLE processes
2. Pick a random winning ticket: `winner = random() % total_tickets`
3. Iterate through processes accumulating tickets
4. Select the first process where `accumulated_tickets > winner`
5. Run selected process and increment its tick counter

### Example
If we have 3 processes:
- Process A: 30 tickets (tickets 0-29)
- Process B: 20 tickets (tickets 30-49)
- Process C: 10 tickets (tickets 50-59)

Random number 35 is drawn → Process B wins and runs

### Fairness
Over many scheduling decisions, each process receives CPU time proportional to its tickets:
- 30 tickets → ~50% CPU (30/60)
- 20 tickets → ~33% CPU (20/60)
- 10 tickets → ~17% CPU (10/60)

---

## Key Features

✅ **Proportional Share**: Processes get CPU time proportional to tickets
✅ **Ticket Inheritance**: Children inherit parent's ticket count on fork
✅ **Default Tickets**: All processes start with 1 ticket
✅ **Dynamic Adjustment**: Use `settickets()` to change ticket count at runtime
✅ **Statistics**: Track how many times each process has been scheduled

---

## File Modifications Summary

### New Files
- `pstat.h` - Process statistics structure
- `lotterytest.c` - Test program
- `ps.c` - Process listing utility
- `LOTTERY_SCHEDULER.md` - Full documentation
- `test_lottery.sh` - Quick test script

### Modified Kernel Files
- `proc.h` - Added tickets/ticks to proc structure
- `proc.c` - Lottery scheduler, random(), getpinfo()
- `defs.h` - Function declarations
- `syscall.h` - System call numbers
- `syscall.c` - System call dispatch
- `sysproc.c` - System call implementations
- `usys.S` - User-space stubs
- `user.h` - User-space declarations
- `Makefile` - Added test programs

---

## Common Issues

### Build Errors
```bash
make clean
make
make fs.img
```

### Test Program Not Found
```bash
# Rebuild filesystem
make clean
make fs.img
```

### Scheduler Not Working
Check that processes have different ticket counts:
```bash
$ ps
```

---

## Performance Notes

- **Overhead**: O(n) per scheduling decision (where n = number of processes)
- **Randomness**: Uses LCG (Linear Congruential Generator)
- **Variance**: Actual ratios may vary due to probabilistic nature
- **Fairness**: Converges to expected ratios over many scheduling decisions

---

## Next Steps

After testing lottery scheduling:
1. ✅ Verify test programs work correctly
2. ✅ Understand the implementation
3. 🔲 Move to Virtual Memory project
4. 🔲 Then Kernel Threads project

---

For detailed implementation information, see [LOTTERY_SCHEDULER.md](LOTTERY_SCHEDULER.md)
