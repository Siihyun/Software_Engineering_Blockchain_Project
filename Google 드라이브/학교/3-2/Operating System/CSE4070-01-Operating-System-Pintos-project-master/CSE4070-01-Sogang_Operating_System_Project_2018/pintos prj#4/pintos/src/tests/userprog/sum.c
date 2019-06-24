/* Tests argument passing to child processes. */

#include <syscall.h>
#include "tests/main.h"

void
test_main (void) 
{
  printf("%d\n", sum_of_four_integer());
  //wait (exec ("child-args childarg"));
}
