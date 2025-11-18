#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}




/**
 * @brief Retrieves the process ID of the parent process (PPID).
 * @return Parent PID, or 0 if the parent does not exist (e.g., init process).
 */
uint64
sys_getppid(void)
{
  struct proc *p = myproc();
  
  if (p->parent) {
    return p->parent->pid;
  }
  return 0;
}

/**
 * @brief Copies the name of the current process into a user-provided buffer.
 * @return 0 on success, -1 on failure (e.g., invalid address or page fault).
 */
uint64
sys_getpname(void)
{
  uint64 addr;
  struct proc *p = myproc();

  // Fetch the destination address from the 0th system call argument.
  argaddr(0, &addr);

  // Safely copy the process name from kernel space to user space.
  // copyout() handles virtual-to-physical address translation via the page table.
  if(copyout(p->pagetable, addr, p->name, 16) < 0)
    return -1;

  return 0;
}