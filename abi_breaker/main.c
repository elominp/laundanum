#include <string.h>
#include <stdio.h>
#include "a.h"

void fillBoom(Boom *);

int main(int argc, char **argv) {
  Boom b;
  memset(&b, 0, sizeof(b));
  fillBoom(&b);
  printf("b.foo = %s\nb.bar = %s\n", b.foo, b.bar);
  return 0;
}
