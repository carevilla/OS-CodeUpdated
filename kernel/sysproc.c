#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
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
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  
  //if(growproc(n) < 0)
    //return -1;

  int newsz = addr + n;
  if(newsz < TRAPFRAME){
  	//allocate more virtual mem
  	myproc()->sz = newsz;
  	return addr;
  }
  return -1;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
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

// return the number of active processes in the system
// fill in user-provided data structure with pid,state,sz,ppid,name
uint64
sys_getprocs(void)
{
  uint64 addr;  // user pointer to struct pstat

  if (argaddr(0, &addr) < 0)
    return -1;
  return(procinfo(addr));
}

//calls the freeCount method implemented in kalloc.c
uint64
sys_freepmem(void){
	int freePages = freeCount() * 4096;
	return freePages;
}


// Hw6 Sem operations below

uint64
sys_sem_init(void){
  uint64 temp;
  int index;
  int val;
  int pshared;

  if (argaddr(0,&temp) < 0 || argint(1,&pshared) < 0 || argint(2, &val) < 0){
    return -1;
  }
  
  if (!pshared ) return -1;

  index = semalloc();
  semtable.sem[index].count = val;

  if (copyout(myproc()->pagetable,temp,(char*)&index,sizeof(index))<0){
    return -1;
  }
  
  return 0;
}

uint64
sys_sem_destroy(void){
  uint64 temp;
  int addr;

  if (argaddr(0,&temp) < 0){
    return -1;
  }
  acquire(&semtable.lock);

  if (copyin(myproc()->pagetable,(char*)&addr,temp,sizeof(int)) < 0){
    release(&semtable.lock);
    return -1;
  }
  semdealloc(addr);
  release(&semtable.lock);
  return 0;
}

uint64
sys_sem_wait(void){
  uint64 temp;
  int addr;

  if (argaddr(0,&temp) || copyin(myproc()->pagetable,(char*)&addr,temp,sizeof(int)) < 0){
    return -1;
  }
  acquire(&semtable.sem[addr].lock);

  while(&semtable.sem[addr].count == 0){
    sleep((void*)&semtable.sem[addr], &semtable.sem[addr].lock);
  }
  semtable.sem[addr].count --;
  release(&semtable.sem[addr].lock);
  return 0;
}

uint64
sys_sem_post(void){
  uint64 temp;
  int addr;

  if (argaddr(0,&temp) || copyin(myproc()->pagetable,(char*)&addr,temp,sizeof(int)) < 0){
    return -1;
  }
  acquire(&semtable.sem[addr].lock);

  semtable.sem[addr].count ++;
  wakeup((void*)&semtable.sem[addr]);

  release(&semtable.sem[addr].lock);
  
  return 0;
}
