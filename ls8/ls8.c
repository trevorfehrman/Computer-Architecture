#include <stdio.h>
#include "cpu.h"

/**
 * Main
 */
int main(int argc, char *argv[])
{
  if (!argv[1])
  {
    printf("Specify a program to execute you ding dong.");
    return 0;
  }
  struct cpu cpu;

  cpu_init(&cpu);
  cpu_load(&cpu, argv[1]);
  cpu_run(&cpu);

  return 0;
}