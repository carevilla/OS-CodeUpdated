#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "spinlock.h"
#define NSEM 100

struct semtab semtable;

void
seminit(void) {
  initlock(&semtable.lock, "semtable");
  for(int i=0; i<NSEM; i++)
    initlock(&semtable.sem[i].lock, "sem");
};

int
semalloc() {
  acquire(&semtable.lock);
  for (int i = 0; i < NSEM; i++){
    if(semtable.sem[i].valid == 0){
      semtable.sem[i].valid = 1;
      release(&semtable.lock);
      return i;
    }
  }
  release(&semtable.lock);
  return -1;
}
void
semdealloc(int curIndex){
  acquire(&semtable.sem[curIndex].lock);
  if(curIndex >= 0 && curIndex < NSEM){
    semtable.sem[curIndex].valid = 0;
    semtable.sem[curIndex].count = 0;
  }
  release(&semtable.sem[curIndex].lock);
}

  
