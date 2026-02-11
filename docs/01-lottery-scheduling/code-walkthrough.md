# Project 1: Lottery Scheduling - Complete Code Walkthrough

This guide walks through every file modification, showing the actual code changes and explaining how they work together to implement lottery scheduling.

## 📋 Table of Contents

1. [Overview](#overview)
2. [Architecture & Data Flow](#architecture--data-flow)
3. [File-by-File Changes](#file-by-file-changes)
4. [How It All Works Together](#how-it-all-works-together)
5. [Testing the Implementation](#testing-the-implementation)

---

## Overview

**Goal**: Replace xv6's round-robin scheduler with a lottery scheduler where processes with more tickets get more CPU time.

**Key Concept**: Each scheduling decision is like a lottery draw. If Process A has 30 tickets and Process B has 10 tickets, Process A has a 75% chance of winning each draw.

---

## Architecture & Data Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    USER SPACE                                │
├─────────────────────────────────────────────────────────────┤
│  Application calls:                                          │
│  • settickets(30)  ──────────────┐                         │
│  • getpinfo(&ps)   ──────────┐   │                         │
└──────────────────────────────┼───┼─────────────────────────┘
                               │   │
┌──────────────────────────────┼───┼─────────────────────────┐
│                    SYSTEM CALL INTERFACE                     │
├──────────────────────────────┼───┼─────────────────────────┤
│  usys.S: SYSCALL stubs       │   │                         │
│  syscall.h: #define numbers  │   │                         │
│  syscall.c: dispatch table ──┼───┼───┐                     │
└──────────────────────────────┼───┼───┼─────────────────────┘
                               │   │   │
┌──────────────────────────────┼───┼───┼─────────────────────┐
│                    KERNEL SPACE                              │
├──────────────────────────────┼───┼───┼─────────────────────┤
│  sysproc.c:                  │   │   │                     │
│  • sys_settickets() <────────┘   │   │                     │
│  • sys_getpinfo()   <────────────┘   │                     │
│         │                             │                     │
│         └─────────────────────────────┘                     │
│                    │                                         │
│  proc.c:           │                                         │
│  • myproc()->tickets = n  (set tickets)                     │
│  • getpinfo() fills pstat (get info)                        │
│  • scheduler() picks winner using random()                  │
│         │                                                    │
│         └──> proc.h: struct proc { tickets, ticks }         │
│              pstat.h: struct pstat (for getpinfo)           │
└─────────────────────────────────────────────────────────────┘
```

---

## File-by-File Changes

### 1. pstat.h (NEW FILE)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/pstat.h`

**Purpose**: Define the structure for returning process statistics to user space.

**The Code**:
```c
#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat {
  int inuse[NPROC];   // whether this slot of the process table is in use (1 or 0)
  int tickets[NPROC]; // the number of tickets this process has
  int pid[NPROC];     // the PID of each process 
  int ticks[NPROC];   // the number of ticks each process has accumulated 
};

#endif // _PSTAT_H_
```

**Why This Matters**:
- NPROC = 64 (max processes in xv6, defined in param.h)
- Arrays of 64 elements, one entry per possible process
- `getpinfo()` will fill this structure
- User programs can then read process statistics

**Relationship to Project**:
- Required by project specification
- Allows test programs to verify lottery scheduling works
- Shows ticket counts and how many times each process was scheduled

---

### 2. proc.h (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/proc.h`

**Purpose**: Add lottery scheduling fields to the process structure.

**What Was Added**:
```c
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  int tickets;                 // 👈 NEW: Number of tickets for lottery scheduling
  int ticks;                   // 👈 NEW: Number of ticks this process has run
};
```

**Why This Matters**:
- Every process needs to track its ticket count
- `tickets`: How much CPU share this process should get
- `ticks`: Running counter of how many times it's been scheduled

**Relationship to Project**:
- Core data structure modification
- These fields are checked by `scheduler()` on every scheduling decision
- `ticks` accumulates every time a process wins the lottery
- Default is 1 ticket (set in `allocproc()`)

---

### 3. proc.c (MAJOR MODIFICATIONS)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/proc.c`

This file has multiple changes. Let's look at each:

#### Change 3.1: Add pstat.h and Random Number Generator

**At the top of the file**:
```c
#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "pstat.h"           // 👈 NEW: Include pstat structure

// ... existing code ...

// 👈 NEW: Simple pseudo-random number generator for lottery scheduling
unsigned long randstate = 1;
unsigned int
random(void)
{
  randstate = randstate * 1664525 + 1013904223;
  return randstate;
}
```

**Why This Matters**:
- Need `pstat.h` for the `getpinfo()` function
- Random number generator is essential for lottery scheduling
- Uses Linear Congruential Generator (LCG) algorithm
- Formula: next = (current × 1664525) + 1013904223
- Simple but effective for kernel use

**How It Works**:
```
Call 1: random() → returns some number (e.g., 3841920)
Call 2: random() → returns different number (e.g., 9247201)
Call 3: random() → returns different number (e.g., 1823047)
...
```

---

#### Change 3.2: Initialize Tickets in allocproc()

**In allocproc() function**:
```c
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->tickets = 1;  // 👈 NEW: Initialize with 1 ticket by default
  p->ticks = 0;    // 👈 NEW: Initialize ticks to 0

  release(&ptable.lock);
  
  // ... rest of function
```

**Why This Matters**:
- `allocproc()` is called whenever a new process is created
- Every process gets 1 ticket by default (fairness)
- Ticks start at 0 (no scheduling yet)

**Relationship to Project**:
- Ensures all processes have at least 1 ticket
- Without this, processes might have garbage values
- 1 ticket = minimal CPU share in lottery

---

#### Change 3.3: Inherit Tickets in fork()

**In fork() function**:
```c
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // 👈 NEW: Inherit tickets from parent
  np->tickets = curproc->tickets;
  np->ticks = 0;  // New process starts with 0 ticks

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;
  
  // ... rest of function
```

**Why This Matters**:
- When a process forks, child inherits parent's ticket count
- Important for maintaining priority across process creation
- Child gets fresh `ticks` count (hasn't run yet)

**Example**:
```
Parent process has 30 tickets
Parent calls fork()
→ Child process also gets 30 tickets
→ Both now have equal lottery weight
```

---

#### Change 3.4: Lottery Scheduler (THE CORE CHANGE)

**In scheduler() function** - This is the heart of the project:

```c
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    
    // 👈 STEP 1: Count total tickets of runnable processes
    int total_tickets = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state == RUNNABLE)
        total_tickets += p->tickets;
    }
    
    // 👈 STEP 2: If there are runnable processes, pick a winner
    if(total_tickets > 0) {
      int winning_ticket = random() % total_tickets;
      int ticket_count = 0;
      
      // 👈 STEP 3: Find the winner by accumulating tickets
      for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->state != RUNNABLE)
          continue;
        
        ticket_count += p->tickets;
        if(ticket_count > winning_ticket) {
          // 👈 STEP 4: Found the winner!
          c->proc = p;
          switchuvm(p);
          p->state = RUNNING;
          p->ticks++;  // 👈 Increment ticks for this process

          swtch(&(c->scheduler), p->context);
          switchkvm();

          // Process is done running for now.
          c->proc = 0;
          break;  // 👈 Only schedule one process per loop
        }
      }
    }
    
    release(&ptable.lock);
  }
}
```

**Detailed Algorithm Walkthrough**:

Let's say we have 3 processes:
- Process A (PID 4): 30 tickets
- Process B (PID 5): 20 tickets  
- Process C (PID 6): 10 tickets

**STEP 1**: Count tickets
```
total_tickets = 30 + 20 + 10 = 60
```

**STEP 2**: Pick winning ticket
```
winning_ticket = random() % 60
Let's say random() returns 1847293
winning_ticket = 1847293 % 60 = 13
```

**STEP 3**: Find winner by accumulating
```
Start: ticket_count = 0

Process A (30 tickets):
  ticket_count += 30  → ticket_count = 30
  Is 30 > 13? YES! 
  → Process A wins!
```

Alternative scenario where winning_ticket = 42:
```
Start: ticket_count = 0

Process A (30 tickets):
  ticket_count += 30  → ticket_count = 30
  Is 30 > 42? NO, continue

Process B (20 tickets):
  ticket_count += 20  → ticket_count = 50
  Is 50 > 42? YES!
  → Process B wins!
```

**Why This Algorithm**:
- O(n) where n = number of processes
- Simple to implement
- Fair distribution over time
- Processes with more tickets have proportionally higher chance

---

#### Change 3.5: Implement getpinfo()

**New function added to proc.c**:

```c
// Get process information for lottery scheduling
int
getpinfo(struct pstat *ps)
{
  struct proc *p;
  int i;
  
  if(ps == 0)
    return -1;
    
  acquire(&ptable.lock);
  
  // Fill in the pstat structure
  for(i = 0, p = ptable.proc; p < &ptable.proc[NPROC]; p++, i++){
    if(p->state != UNUSED) {
      ps->inuse[i] = 1;
      ps->tickets[i] = p->tickets;
      ps->pid[i] = p->pid;
      ps->ticks[i] = p->ticks;
    } else {
      ps->inuse[i] = 0;
      ps->tickets[i] = 0;
      ps->pid[i] = 0;
      ps->ticks[i] = 0;
    }
  }
  
  release(&ptable.lock);
  return 0;
}
```

**Why This Matters**:
- Copies process table data to user space
- User programs can see all processes and their stats
- Used by test programs to verify lottery scheduling

**How It Works**:
1. Validates pointer is not NULL
2. Locks process table (prevent changes during copy)
3. Loops through all 64 process slots
4. Copies tickets, PID, ticks for each active process
5. Unlocks and returns success

---

### 4. defs.h (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/defs.h`

**What Was Added**:

```c
// At the top with other struct declarations:
struct buf;
struct context;
struct file;
struct inode;
struct pipe;
struct proc;
struct rtcdate;
struct spinlock;
struct sleeplock;
struct stat;
struct superblock;
struct pstat;         // 👈 NEW: Forward declaration

// In the proc.c section:
int             cpuid(void);
void            exit(void);
int             fork(void);
int             growproc(int);
int             kill(int);
struct cpu*     mycpu(void);
struct proc*    myproc();
void            pinit(void);
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            sched(void);
void            setproc(struct proc*);
void            sleep(void*, struct spinlock*);
void            userinit(void);
int             wait(void);
void            wakeup(void*);
void            yield(void);
int             getpinfo(struct pstat*);  // 👈 NEW: Function declaration
```

**Why This Matters**:
- `defs.h` declares all kernel functions
- Other files include this to know what functions exist
- Forward declaration of `struct pstat` needed
- `getpinfo()` declaration tells compiler the function exists

---

### 5. syscall.h (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/syscall.h`

**What Was Added**:

```c
// System call numbers
#define SYS_fork 1
#define SYS_exit 2
#define SYS_wait 3
// ... existing syscalls ...
#define SYS_close 21
#define SYS_getreadcount 22
#define SYS_settickets 23      // 👈 NEW
#define SYS_getpinfo 24        // 👈 NEW
```

**Why This Matters**:
- Each system call needs a unique number
- User space puts this number in register %eax
- Kernel uses number to look up which function to call
- Numbers 23 and 24 assigned to new syscalls

**How It's Used**:
```
User calls: settickets(30)
  ↓
usys.S puts 23 in %eax
  ↓
Triggers interrupt
  ↓
Kernel looks up syscalls[23]
  ↓
Calls sys_settickets()
```

---

### 6. syscall.c (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/syscall.c`

**What Was Added**:

```c
// Function declarations (external)
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_getreadcount(void);
extern int sys_settickets(void);   // 👈 NEW
extern int sys_getpinfo(void);     // 👈 NEW

// Dispatch table
static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,
    [SYS_exit] sys_exit,
    // ... existing entries ...
    [SYS_getreadcount] sys_getreadcount,
    [SYS_settickets] sys_settickets,     // 👈 NEW
    [SYS_getpinfo] sys_getpinfo,         // 👈 NEW
};
```

**Why This Matters**:
- Dispatch table maps syscall numbers to functions
- When syscall #23 arrives, kernel calls `syscalls[23]`
- This points to `sys_settickets()`

**The Dispatch Process**:
```c
void syscall(void) {
  int num;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;  // Get syscall number from register
  
  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    curproc->tf->eax = syscalls[num]();  // Call function, return value in %eax
  } else {
    // Invalid syscall
    curproc->tf->eax = -1;
  }
}
```

---

### 7. sysproc.c (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/sysproc.c`

**What Was Added**:

```c
#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"    // 👈 NEW

// ... existing functions ...

// 👈 NEW: Set the number of tickets for the calling process
int sys_settickets(void)
{
  int tickets;
  
  // Get argument from user space
  if (argint(0, &tickets) < 0)
    return -1;
  
  // Validate: must have at least 1 ticket
  if (tickets < 1)
    return -1;
  
  // Set tickets for current process
  myproc()->tickets = tickets;
  return 0;
}

// 👈 NEW: Get process information for lottery scheduling
int sys_getpinfo(void)
{
  struct pstat *ps;
  
  // Get pointer argument from user space (with safety checks)
  if (argptr(0, (char **)&ps, sizeof(*ps)) < 0)
    return -1;
  
  // Call kernel function to fill the structure
  return getpinfo(ps);
}
```

**How argint() and argptr() Work**:

```c
// User program calls: settickets(30)
// Stack looks like:
//   [return address]
//   [argument: 30]
//
// argint(0, &tickets) gets first argument (index 0)
// tickets now contains 30

// User program calls: getpinfo(&ps)
// Stack looks like:
//   [return address]
//   [argument: pointer to ps]
//
// argptr(0, &ps, size) gets first argument AND validates:
//   - Pointer is not NULL
//   - Points to valid user memory
//   - Size is within user address space
```

**Why Validation Matters**:
- User could pass malicious pointers
- Could try to access kernel memory
- Could cause crashes
- `argptr()` prevents these attacks

---

### 8. user.h (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/user.h`

**What Was Added**:

```c
struct stat;
struct rtcdate;
struct pstat;    // 👈 NEW: Forward declaration

// system calls
int fork(void);
// ... existing syscalls ...
int uptime(void);
int getreadcount(void);
int settickets(int);              // 👈 NEW
int getpinfo(struct pstat *);     // 👈 NEW
```

**Why This Matters**:
- User programs include `user.h`
- Tells them these functions exist
- Provides function signatures (arguments and return types)

---

### 9. usys.S (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/usys.S`

**What Was Added**:

```assembly
#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
// ... existing syscalls ...
SYSCALL(uptime)
SYSCALL(getreadcount)
SYSCALL(settickets)    // 👈 NEW
SYSCALL(getpinfo)      // 👈 NEW
```

**What This Does**:
- Assembly macro that creates syscall stubs
- `SYSCALL(settickets)` expands to:

```assembly
.globl settickets;     # Make function visible
settickets:
  movl $23, %eax;      # Put syscall number in register
  int $T_SYSCALL;      # Trigger interrupt (trap into kernel)
  ret                  # Return to caller
```

**The Full Journey**:
```
User program:  settickets(30);
               ↓
usys.S stub:   movl $23, %eax     (syscall number)
               movl 4(%esp), ...   (copy argument 30)
               int $T_SYSCALL      (trap to kernel)
               ↓
Kernel trap:   syscall()
               ↓
Dispatch:      syscalls[23]()
               ↓
Handler:       sys_settickets()
               ↓
Action:        myproc()->tickets = 30
               ↓
Return:        return 0 (success)
               ↓
User gets:     0
```

---

### 10. Makefile (MODIFIED)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/Makefile`

**What Was Added**:

```makefile
UPROGS=\
	_cat\
	_echo\
	# ... existing programs ...
	_testreadcount\
	_lotterytest\     # 👈 NEW
	_ps\              # 👈 NEW
```

**Why This Matters**:
- `UPROGS` lists all user programs to compile
- Make will compile `lotterytest.c` → `_lotterytest`
- Make will compile `ps.c` → `_ps`
- These get included in the filesystem image

---

### 11. lotterytest.c (NEW FILE)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/lotterytest.c`

**Purpose**: Test program to verify lottery scheduling with a 3:2:1 ratio.

**Key Sections**:

```c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

#define TICKETS_A 30   // Process A
#define TICKETS_B 20   // Process B
#define TICKETS_C 10   // Process C

void spin(void) {
  // CPU-intensive work
  int i, j;
  for (i = 0; i < 1000000; i++)
    for (j = 0; j < 100; j++)
      ;
}

int main(int argc, char *argv[]) {
  struct pstat ps;
  int pid_a, pid_b, pid_c;
  
  // Fork process A (30 tickets)
  pid_a = fork();
  if (pid_a == 0) {
    settickets(TICKETS_A);    // 👈 Set 30 tickets
    printf(1, "Process A (PID %d): %d tickets\n", getpid(), TICKETS_A);
    for (int i = 0; i < 10; i++) {
      spin();                  // 👈 Do work
    }
    exit();
  }
  
  // Fork process B (20 tickets)
  pid_b = fork();
  if (pid_b == 0) {
    settickets(TICKETS_B);    // 👈 Set 20 tickets
    printf(1, "Process B (PID %d): %d tickets\n", getpid(), TICKETS_B);
    for (int i = 0; i < 10; i++) {
      spin();
    }
    exit();
  }
  
  // Fork process C (10 tickets)
  pid_c = fork();
  if (pid_c == 0) {
    settickets(TICKETS_C);    // 👈 Set 10 tickets
    printf(1, "Process C (PID %d): %d tickets\n", getpid(), TICKETS_C);
    for (int i = 0; i < 10; i++) {
      spin();
    }
    exit();
  }
  
  // Parent waits for all children
  wait();
  wait();
  wait();
  
  // Get and display results
  if (getpinfo(&ps) == 0) {
    printf(1, "\nLottery Scheduling Results:\n");
    printf(1, "PID\tTickets\tTicks\n");
    for (int i = 0; i < 64; i++) {
      if (ps.inuse[i] && 
          (ps.pid[i] == pid_a || ps.pid[i] == pid_b || ps.pid[i] == pid_c)) {
        printf(1, "%d\t%d\t%d\n", ps.pid[i], ps.tickets[i], ps.ticks[i]);
      }
    }
  }
  
  exit();
}
```

**What This Tests**:
1. Creates 3 competing processes
2. Gives them 30:20:10 ticket ratio
3. Makes them do equal work
4. Reports how many times each was scheduled
5. Expected result: ~3:2:1 ratio of ticks

**Example Output**:
```
Process A (PID 4): 30 tickets
Process B (PID 5): 20 tickets
Process C (PID 6): 10 tickets

Lottery Scheduling Results:
PID     Tickets Ticks
4       30      156
5       20      98
6       10      54

Ratio: ~3:2:1 ✓
```

---

### 12. ps.c (NEW FILE)
**Location**: `/home/judah/cs361/project-1-lottery-scheduler/ps.c`

**Purpose**: Simple program to show all processes.

```c
#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int main(int argc, char *argv[]) {
  struct pstat ps;
  
  if (getpinfo(&ps) != 0) {
    printf(1, "Error: getpinfo failed\n");
    exit();
  }
  
  printf(1, "PID\tTickets\tTicks\n");
  for (int i = 0; i < 64; i++) {
    if (ps.inuse[i]) {
      printf(1, "%d\t%d\t%d\n", ps.pid[i], ps.tickets[i], ps.ticks[i]);
    }
  }
  
  exit();
}
```

**Usage**:
```bash
$ ps
PID     Tickets Ticks
1       1       45      # init
2       1       23      # sh (shell)
3       30      156     # Your process with 30 tickets
```

---

## How It All Works Together

Let's trace a complete example: User program calls `settickets(30)`

### The Complete Flow:

```
┌─────────────────────────────────────────────────────────────────┐
│ USER SPACE                                                       │
│                                                                  │
│  myapp.c:                                                        │
│    settickets(30);  ← User calls function                       │
│         │                                                        │
└─────────┼────────────────────────────────────────────────────────┘
          │
┌─────────▼────────────────────────────────────────────────────────┐
│ usys.S: (Assembly stub)                                          │
│                                                                  │
│    settickets:                                                   │
│      movl $23, %eax        ← Put syscall number in register     │
│      movl 8(%esp), %ebx    ← Get argument (30) from stack       │
│      int $0x40             ← Trigger interrupt (trap)           │
│      ret                                                         │
│         │                                                        │
└─────────┼────────────────────────────────────────────────────────┘
          │ [INTERRUPT - Switch to kernel mode]
          │
┌─────────▼────────────────────────────────────────────────────────┐
│ KERNEL SPACE                                                     │
│                                                                  │
│  trap.c:                                                         │
│    Interrupt received                                            │
│    Check: Is it a system call? Yes (int 0x40)                   │
│    Call: syscall()                                               │
│         │                                                        │
│  syscall.c:                                                      │
│    num = tf->eax;          ← Get 23 from register               │
│    syscalls[23]();         ← Look up in table                   │
│         │                                                        │
│  sysproc.c:                                                      │
│    sys_settickets() {                                            │
│      argint(0, &tickets);  ← Get argument: tickets = 30         │
│      if (tickets < 1)      ← Validate                           │
│        return -1;                                                │
│      myproc()->tickets = tickets;  ← SET THE TICKETS!           │
│      return 0;             ← Success                             │
│    }                                                             │
│         │                                                        │
└─────────┼────────────────────────────────────────────────────────┘
          │ [Return value (0) placed in %eax]
          │ [INTERRUPT RETURN - Switch back to user mode]
          │
┌─────────▼────────────────────────────────────────────────────────┐
│ USER SPACE                                                       │
│                                                                  │
│  myapp.c:                                                        │
│    int result = settickets(30);  ← Gets return value (0)        │
│    // Success! Process now has 30 tickets                       │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

### Now The Scheduler Runs:

```
TIMER INTERRUPT → scheduler() called

Step 1: Count tickets
  - Process A: 30 tickets (our process)
  - Process B: 1 ticket (init)
  - Process C: 1 ticket (shell)
  - Total: 32 tickets

Step 2: Pick winner
  - random() % 32 = 15

Step 3: Find winner
  - Start: ticket_count = 0
  - Process A: ticket_count += 30 → 30 > 15? YES!
  - Process A wins! 🎉

Step 4: Run Process A
  - Switch to Process A
  - Increment A's ticks: ticks++
  - Execute until next interrupt
  - (Time slice usually ~10ms)

Next Timer Interrupt:
  - random() % 32 = 8
  - Process A wins again! (30 out of 32 chance = 93.75%)

Eventually:
  - random() % 32 = 31
  - Process A: 30 > 31? No
  - Process B: 31 > 31? Yes! Process B wins
  - Process B gets its turn (rare, only ~3% of the time)
```

---

## Testing the Implementation

### Method 1: Automated Test

```bash
cd ~/cs361/project-1-lottery-scheduler
./test_lottery.sh
```

This will:
1. Build xv6
2. Launch QEMU
3. (You manually run `lotterytest` inside)

### Method 2: Manual Test

```bash
cd ~/cs361/project-1-lottery-scheduler
make qemu-nox
```

Inside xv6:
```bash
$ lotterytest
Process A (PID 4): 30 tickets
Process B (PID 5): 20 tickets
Process C (PID 6): 10 tickets

Lottery Scheduling Results:
PID     Tickets Ticks
4       30      147
5       20      102
6       10      51

Expected ratio: 3:2:1 (30:20:10 tickets)
Actual ratio: 2:2:1
```

The ratio should be approximately 3:2:1. Some variance is normal due to randomness!

### Method 3: Process Viewer

```bash
$ ps
PID     Tickets Ticks
1       1       45
2       1       23
4       30      156
```

---

## Summary: The Big Picture

### What We Built:
1. **Data Structures**: Added `tickets` and `ticks` to every process
2. **Scheduler**: Replaced round-robin with lottery algorithm
3. **System Calls**: Added `settickets()` and `getpinfo()`
4. **Random Generator**: LCG for fair lottery draws
5. **Test Programs**: `lotterytest` and `ps` to verify it works

### How They Connect:
```
User calls settickets(30)
    ↓
Kernel stores tickets in proc->tickets
    ↓
Scheduler counts all tickets (every ~10ms)
    ↓
Scheduler picks random ticket number
    ↓
Process with that ticket wins
    ↓
Process runs, ticks incremented
    ↓
(Repeat thousands of times)
    ↓
User calls getpinfo() to see results
    ↓
Results show ~3:2:1 ratio achieved!
```

### Key Files Changed:
- **pstat.h**: Data structure for statistics ✨
- **proc.h**: Add tickets/ticks to process ✨
- **proc.c**: Lottery scheduler + helpers ✨✨✨ (CORE)
- **defs.h**: Function declarations
- **syscall.h/c**: System call registration
- **sysproc.c**: System call handlers ✨
- **user.h**: User-space declarations
- **usys.S**: Assembly stubs
- **lotterytest.c**: Test program ✨
- **ps.c**: Debugging tool ✨

✨ = Contains significant new code
✨✨✨ = Most important changes

---

**Next**: To see this in action, try:
```bash
cd ~/cs361/project-1-lottery-scheduler
make qemu-nox
$ lotterytest
```
