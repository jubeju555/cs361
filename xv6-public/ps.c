// Simple ps-like program to show process information
// including lottery scheduling statistics

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int main(int argc, char *argv[])
{
    struct pstat ps;

    if (getpinfo(&ps) != 0)
    {
        printf(1, "Error: getpinfo failed\n");
        exit();
    }

    printf(1, "PID\tTickets\tTicks\n");
    for (int i = 0; i < 64; i++)
    {
        if (ps.inuse[i])
        {
            printf(1, "%d\t%d\t%d\n", ps.pid[i], ps.tickets[i], ps.ticks[i]);
        }
    }

    exit();
}
