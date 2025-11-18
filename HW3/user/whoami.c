#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Student ID: 2014142130\n");
  printf("Student name: Seongjung Um\n");

  volatile int k = 0; 
  int i, j;

  // [Dynamic Workload Simulation]
  // Alternate between CPU bursts and I/O waits 3 times
  // to induce transitions: Q2 -> Q1 -> Q0 -> Q2
  for (j = 0; j < 3; j++) {
      
      // 1. CPU Burst (Approx. 30M iterations)
      // Consume the entire time slice to trigger demotion (Q2 -> Q1)
      for(i = 0; i < 30000000; i++){
          k = i * i;
      }

      // 2. Short I/O Wait (1 tick)
      // Enter Q0 (Wait/Sleep) state, then promote to Q2 upon wakeup
      pause(1);
  }

  printf("Workload finished (check k=%d)\n", k);

  exit(0);
}