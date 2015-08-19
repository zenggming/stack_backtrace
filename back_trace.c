#include <stdio.h>

/* Define the stack_frame layout */
struct stack_frame {
  struct stack_frame * prev;/* pointing to previous stack_frame */
  long   caller_address;/* the address of caller */
};

/* pointing to the stack_bottom from libc */
extern void * __libc_stack_end;

static int back_trace(long stacks[ ], int size)
{
  void * stack_top;/* pointing to current API stack top */
  struct stack_frame * current_frame;
  int    i, found = 0;

  /* get current stack-frame */
  current_frame = (struct stack_frame*)(__builtin_frame_address(0));
  
  stack_top = &stack_top;/* pointing to curent API's stack-top */
  
  /* Omit current stack-frame due to calling current API 'back_trace' itself */
  for (i = 0; i < 1; i++) {
    if (((void*)current_frame < stack_top) || ((void*)current_frame > __libc_stack_end)) break;
    current_frame = current_frame->prev;
  }
  
  /* As we pointing to chains-beginning of real-callers, let's collect all stuff... */
  for (i = 0; i < size; i++) {
    /* Stop in case we hit the back-stack information */
    if (((void*)current_frame < stack_top) || ((void*)current_frame > __libc_stack_end)) break;
    /* omit some weird caller's stack-frame info * if hits. Avoid dead-loop */
    if ((current_frame->caller_address == 0) || (current_frame == current_frame->prev)) break;
    /* make sure the stack_frame is aligned? */
    if (((unsigned long)current_frame) & 0x01) break;

    /* Ok, we can collect the guys right now... */
    stacks[found++] = current_frame->caller_address;
    /* move to previous stack-frame */
    current_frame = current_frame->prev;
  }

  /* omit the stack-frame before main, like API __libc_start_main */
  if (found > 1) found--;

  stacks[found] = 0;/* fill up the ending */

  return found;
}

/* below is the example testing code for stack-race */
#define MAX_NUM 100

#define define_func(value) \
int recursive_sum_##value(int current_value) {\
  if (current_value == 0) {\
    long stacks[MAX_NUM + 1];\
    int found = back_trace(stacks, MAX_NUM);\
    return 0;\
  }\
  return (current_value + recursive_sum_next(current_value - 1));\
}

/* Using this to show different caller functions names like recursive_sum_0... recursive_sum_7... */
static int recursive_sum_next(int value)
{
  switch (value) {
    case 0: return 0;
    case 1: return value + recursive_sum_0(value - 1);
    case 2: return value + recursive_sum_1(value - 1);
    case 3: return value + recursive_sum_2(value - 1);
    case 4: return value + recursive_sum_3(value - 1);
    case 5: return value + recursive_sum_4(value - 1);
    case 6: return value + recursive_sum_5(value - 1);
    case 7: return value + recursive_sum_6(value - 1);
    case 8: return value + recursive_sum_7(value - 1);
    case 9: return value + recursive_sum_8(value - 1);
    case 10: return value + recursive_sum_9(value - 1);
    default: break;
  }
  return 0;
}

define_func(0);
define_func(1);
define_func(2);
define_func(3);
define_func(4);
define_func(5);
define_func(6);
define_func(7);
define_func(8);
define_func(9);
define_func(10);

int main(void) {
  int sum = 0, i = 10;
  
  sum = recursive_sum_10(10);
  
  printf("Sum(%d)=%d\n", i, sum);
  return 0;
}
