#include <stdio.h>

extern unsigned long long asm_multiply(void);

int main(void)
{
  unsigned long long product = asm_multiply();
  printf("Product = %llu\n", product);

  return 0;
}
