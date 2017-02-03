#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include "libx.h"
#include "state_machine.h"

struct test_sm
{
  unsigned char changed; 

  enum {
    TEST_INITIALIZE,
    TEST_RUNNING,
    TEST_STOP,
  } TEST_state;
};

#define STATE_MACHINE_DATA         struct test_sm
#define STATE_MACHINE_DEBUG_PREFIX "TEST_SM"

SM_STATE(TEST, INITIALIZE)
{
  SM_ENTRY(TEST, INITIALIZE);
  printf("INITIALIZE\n");
}

SM_STATE(TEST, RUNNING)
{
  SM_ENTRY(TEST, RUNNING);
  printf("RUNNING\n");
}

SM_STATE(TEST, STOP)
{
  SM_ENTRY(TEST, STOP);
  printf("STOP\n");
  sm->changed = false;
}

static void test_step_local(struct test_sm *sm)
{
  printf("sm->TEST_state: %d\n", sm->TEST_state);
  switch(sm->TEST_state) 
  {
    case TEST_INITIALIZE:
      SM_ENTER(TEST, RUNNING);
      break;
    case TEST_RUNNING:
      SM_ENTER(TEST, STOP);
      break;
    case TEST_STOP:
      SM_ENTER(TEST, INITIALIZE);
      break;
    default:
      break;
  }
}

SM_STEP(TEST)
{
  test_step_local(sm);
}

int test_sm_step(struct test_sm *sm)
{
  int res = 0;
  
  do
  {
    sm->changed = FALSE;
    SM_STEP_RUN(TEST);
    if(sm->changed)
      res = 1;
  }while(sm->changed);
  
  return res;
}

void test_sm_func()
{
  struct test_sm sm;
  
  memset(&sm, 0, sizeof(sm));
  
  test_sm_step(&sm);
}
