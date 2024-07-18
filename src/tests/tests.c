#include "tests.h"

int main() {
  int result = 0;

  result += affine_tests();
  putchar('\n');
  result += obj_test();
  putchar('\n');

  return result == 0 ? 0 : 1;
}
