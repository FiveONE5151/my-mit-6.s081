#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// int pgaccess(void *base, int len, void *mask);
// base: starting address of the first page to check
// len: number of pages to check
// mask: buffer to hold the result
#ifdef LAB_PGTBL
int sys_pgaccess(void)
{
  pagetable_t pagetable = myproc()->pagetable;
  uint64 base = 0;
  int len = 0;
  uint64 kmask = 0;
  uint64 mask = 0;

  if (argaddr(0, &base) < 0)
    return -1;
  if (argint(1, &len) < 0)
    return -1;
  if (argaddr(2, &mask) < 0)
    return -1;
  // vmprint(pagetable, 0);

  if (len > 64)
    panic("pgaccess: too much pages");

  for (int i = 0; i < len; i++)
  {
    uint64 tgtva = base + i * PGSIZE; // not +i, because we are checking a page, not an specific item of a page.
                                      // in xv6, memory is accessed by page, that is the offset of va/pa is of no matter.
    pte_t *pte = walk(pagetable, tgtva, 0);
    if (pte == 0)
      panic("pgaccess: pte should exist");
    if ((!(*pte & PTE_V) || !(*pte & PTE_A)))
      continue;
    kmask = kmask | (1 << i); // kmask += 1 << i;
    *pte = *pte & (~PTE_A);   // restore the pte, so that the bit will not be set forever
  }

  if (copyout(pagetable, mask, (char *)&kmask, sizeof(uint64)) != 0)
    return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
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
