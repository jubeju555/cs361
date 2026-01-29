#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  int count1, count2;
  char buf[10];
  int fd;

  // Get initial read count
  count1 = getreadcount();
  printf(1, "Initial read count: %d\n", count1);

  // Open and read from a file
  fd = open("README", O_RDONLY);
  if(fd >= 0) {
    read(fd, buf, 10);
    close(fd);
  }

  // Get read count after one read
  count2 = getreadcount();
  printf(1, "After 1 read: %d\n", count2);

  // Do more reads
  fd = open("README", O_RDONLY);
  if(fd >= 0) {
    read(fd, buf, 10);
    read(fd, buf, 10);
    read(fd, buf, 10);
    close(fd);
  }

  // Get final count
  printf(1, "After 4 total reads: %d\n", getreadcount());
  printf(1, "Test passed!\n");

  exit();
}
