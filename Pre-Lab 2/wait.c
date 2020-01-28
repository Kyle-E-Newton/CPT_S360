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

}

// 2. Implement kwakeup per the algorithm in 3.4.2
int kwakeup(int event)
{

}

// 4. Modify kexit() to give away children to P1:
/*********************
int kexit(int value)
{
  // give away children, if any, to P1
  
  running->exitCode = value;
  running->status = ZOMBIE;
  kwakeup(running->parent);
  tswitch(); 
}
***********************/

// 3. Implement kwait() per the algorithm in 3.5.3
int kwait(int *status)
{

}

// 4. Add a "wait" command to let proc wait for ZOMBIE child
