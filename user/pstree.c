#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

struct pstat uproc[NPROC];
int nprocs;

void mktree(int indent, int pid)
{
  int found = 0;
  int i = 0;

  while (!found && i<nprocs) {
    if (uproc[i].pid == pid)
      found = 1;
    else
      i++;
  }
  if (!found) {
    printf("pid %d not found\n", pid);
    return;
  }
  for (int j=0; j<indent; j++)
    printf("  ");
  printf("%s(%d)\n", uproc[i].name, uproc[i].pid);
  for (i=0; i<nprocs; i++)
    if (uproc[i].ppid == pid) {
      mktree(indent+1, uproc[i].pid);
    }
  return;
}

int
main(int argc, char **argv)
{
  int pid = 1;

  if (argc == 2)
    pid = atoi(argv[1]);
  nprocs = getprocs(uproc);
  if (nprocs < 0)
    exit(-1);
  mktree(0, pid);
  exit(0);
}
