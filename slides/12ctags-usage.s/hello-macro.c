#include <stdio.h>
#define defineEchoFunction(NAME,MESSAGE) \
  int NAME(void) { printf("%s\n", MESSAGE); }

defineEchoFunction (main, "hello, world)
