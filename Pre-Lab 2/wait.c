extern PROC *running;

int kexit(int value)
{
  running->exitCode = value;
  running->status = ZOMBIE;
  tswitch();
}

/********* Required PRE-work ***********/
// 1. Modify kfork() to implement process tree as a BINARY tree

// 2. Implement ksleep() per the algorithm in 3.4.1
int ksleep(int event)
{
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  tswitch();
}

// 2. Implement kwakeup per the algorithm in 3.4.2
int kwakeup(int event)
{
  PROC *p = dequeue(&sleepList);
  while (p != NULL)
  {
    if (p->event == event)
    {
      p->status = READY;
      enqueue(&readyQueue, p);
    }
  }
}

// 4. Modify kexit() to give away children to P1:
int kexit(int value)
{
  // give away children, if any, to P1
  PROC *p;
  if(running->child != NULL) {
    p = &proc[1];
    while(p->child != NULL) {
      p = p->child;
    }
    p->child = running->child;
  }
  running->exitCode = value;
  running->status = ZOMBIE;
  kwakeup(running->parent);
  tswitch(); 
}

// 3. Implement kwait() per the algorithm in 3.5.3
int kwait(int *status)
{
  while(1) {
    for(int i = 0; i < NPROC; i++) {
      PROC *p = &running[i];
      if(p->status == ZOMBIE) {
      status = p->exitCode;
      enqueue(&freeList, p);
      return p->pid;
      }
    }
  }
  return 0;
}

// 4. Add a "wait" command to let proc wait for ZOMBIE child
void do_wait() {
  printf("Waiting\n");
  int *status;
  int pid = kwait(status);
  printf("Finished waiting with proc %d and status %d", pid, status);
}
