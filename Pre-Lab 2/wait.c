extern PROC *running;
extern PROC *freeList;
extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC proc[NPROC];

// int kexit(int value)
// {
//   running->exitCode = value;
//   running->status = ZOMBIE;
//   tswitch();
// }

/********* Required PRE-work ***********/
// 1. Modify kfork() to implement process tree as a BINARY tree
int kfork(int(*func))
{
  int i;
  PROC *pCur = NULL;
  PROC *p = dequeue(&freeList);
  if (!p)
  {
    printf("No more pocesses");
    return (-1);
  }
  p->status = READY;
  p->priority = 1;
  p->ppid = running->pid;

  if (running->child == NULL)
  {
    running->child = p;
  }
  else
  {
    pCur = running->child;

    while (pCur->sibling != NULL)
    {
      pCur = pCur->sibling;
    }

    pCur->sibling = p;
  }

  p->parent = running;
  p->child = NULL;
  p->sibling = NULL;

  //                    -1   -2  -3  -4  -5  -6  -7  -8   -9
  // kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
  for (i = 1; i < 10; i++)
    p->kstack[SSIZE - i] = 0;

  p->kstack[SSIZE - 1] = (int)func;
  p->saved_sp = &(p->kstack[SSIZE - 9]);

  enqueue(&readyQueue, p);

  return p->pid;
}

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
  if (running->pid == 1)
  {
    printf("P1 never dies\n");
    return 0;
  }
  else
  {
    printf("Proc %d in kexit(), value=%d\n", running->pid, value);
    running->exitCode = value;
    running->status = ZOMBIE;

    if (running->pid != 1)
    {
      giveAwayChildren();
    }

    kwakeup(running->parent);
    tswitch();
  }
}

// 3. Implement kwait() per the algorithm in 3.5.3
int kwait(int *status)
{
  PROC *pCur = running->child;
  PROC *pPrev = NULL;

  if (running->child == NULL)
  {
    return -1;
  }

  while (1)
  {
    for (pCur = running->child, pPrev = NULL; pCur; pPrev = pCur, pCur = pCur->sibling)
    {
      printf("%d", pCur->status);
      if (pCur->status == 4)
      {
        *status = pCur->exitCode;
        if (pPrev == NULL)
        {
          running->child = pCur->sibling;
        }
        else
        {
          pPrev->sibling = pCur->sibling;
        }
        pCur->status = 0;

        enqueue(&freeList, pCur);

        return pCur->pid;
      }
    }
  }
}

// 4. Add a "wait" command to let proc wait for ZOMBIE child
void do_wait()
{
  printf("Waiting\n");
  int *status;
  printf(&running);
  printf("Waiting\n");
  int pid = kwait(status);
  printf("Waiting\n");
  printf("Finished waiting with proc %d and status %d", pid, status);
}

void giveAwayChildren()
{
  PROC *p1 = running;
  PROC *pCur = NULL;

  while (p1->pid != 1)
  {
    p1 = p1->parent;
  }

  if (running->child != NULL)
  {
    if (p1->child == NULL)
    {
      running->child = p1->child;
      pCur = p1->child;
    }
    else
    {
      pCur = p1->child;
      while (pCur->sibling != NULL)
      {
        pCur = p1->sibling;
      }
      pCur->sibling = running->child;
      pCur = pCur->sibling;
    }

    while (pCur != NULL)
    {
      pCur->parent = p1;
      pCur->pid = p1->pid;
      pCur = pCur->sibling;
    }
  }
}
