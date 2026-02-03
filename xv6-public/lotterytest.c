// Test program for lottery scheduling
// Creates 3 processes with a 3:2:1 ticket ratio
// and shows how many time slices each receives

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

#define TICKETS_A 30
#define TICKETS_B 20
#define TICKETS_C 10

void
spin(void)
{
  int i, j;
  for (i = 0; i < 1000000; i++)
    for (j = 0; j < 100; j++)
      ;
}

int
main(int argc, char *argv[])
{
  struct pstat ps;
  int pid_a, pid_b, pid_c;
  int my_pid;
  
  // Fork process A (30 tickets)
  pid_a = fork();
  if (pid_a == 0) {
    settickets(TICKETS_A);
    my_pid = getpid();
    printf(1, "Process A (PID %d): %d tickets\n", my_pid, TICKETS_A);
    
    // Do some work
    for (int i = 0; i < 10; i++) {
      spin();
    }
    exit();
  }
  
  // Fork process B (20 tickets)
  pid_b = fork();
  if (pid_b == 0) {
    settickets(TICKETS_B);
    my_pid = getpid();
    printf(1, "Process B (PID %d): %d tickets\n", my_pid, TICKETS_B);
    
    // Do some work
    for (int i = 0; i < 10; i++) {
      spin();
    }
    exit();
  }
  
  // Fork process C (10 tickets)
  pid_c = fork();
  if (pid_c == 0) {
    settickets(TICKETS_C);
    my_pid = getpid();
    printf(1, "Process C (PID %d): %d tickets\n", my_pid, TICKETS_C);
    
    // Do some work
    for (int i = 0; i < 10; i++) {
      spin();
    }
    exit();
  }
  
  // Parent waits for all children
  wait();
  wait();
  wait();
  
  // Get process info and display results
  if (getpinfo(&ps) == 0) {
    printf(1, "\nLottery Scheduling Results:\n");
    printf(1, "PID\tTickets\tTicks\n");
    for (int i = 0; i < 64; i++) {
      if (ps.inuse[i] && (ps.pid[i] == pid_a || ps.pid[i] == pid_b || ps.pid[i] == pid_c)) {
        printf(1, "%d\t%d\t%d\n", ps.pid[i], ps.tickets[i], ps.ticks[i]);
      }
    }
    
    // Calculate ratios
    int ticks_a = 0, ticks_b = 0, ticks_c = 0;
    for (int i = 0; i < 64; i++) {
      if (ps.inuse[i]) {
        if (ps.pid[i] == pid_a) ticks_a = ps.ticks[i];
        if (ps.pid[i] == pid_b) ticks_b = ps.ticks[i];
        if (ps.pid[i] == pid_c) ticks_c = ps.ticks[i];
      }
    }
    
    printf(1, "\nExpected ratio: 3:2:1 (30:20:10 tickets)\n");
    if (ticks_c > 0) {
      printf(1, "Actual ratio: %d:%d:%d\n", 
             ticks_a / ticks_c, ticks_b / ticks_c, 1);
    }
  } else {
    printf(1, "Error: getpinfo failed\n");
  }
  
  exit();
}
