#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



uint32_t uint32_pow(uint32_t base, uint32_t exponent);
uint32_t get_nDigits(uint32_t x);



int main(int argc, char *argv[])
{
  uint32_t ending;
  uint32_t nDigits;
  uint32_t a;
  uint32_t b;
  uint32_t mod;
  uint32_t count = 0;

  if (argc != 3)
  {
    printf("Usage: %s <nDigits> <ending>\n", argv[0]);
    return 1;
  }

  nDigits = (uint32_t)strtoul(argv[1], NULL, 0);
  printf("- allowed number of digits: %" PRIu32 "\n", nDigits);
  if (nDigits == 0) return 0;

  ending = (uint32_t)strtoul(argv[2], NULL, 0);
  printf("- target ending: %" PRIu32 "\n", ending);

  mod = uint32_pow(10, get_nDigits(ending));
  printf("Required modulo: mod %" PRIu32 "\n", mod);

  for(a = 0; a < uint32_pow(10, nDigits); a++)
  {
    for(b = a; b < uint32_pow(10, nDigits); b++)
    {
      if ((a*b) % mod == ending)
      {
        printf("- %" PRIu32 " x %" PRIu32 " = %" PRIu32 "\n", a, b, a*b);
        count++;
      }
    }
  }

  printf("%" PRIu32 " solutions found.\n", count);

  return 0;
}




uint32_t uint32_pow(uint32_t base, uint32_t exponent)
{
  uint32_t result = 1;

  while (exponent > 0)
  {
    if (exponent & 1)
    {
      result *= base;
    }

    base *= base;
    exponent >>= 1;
  }

  return result;
}



uint32_t get_nDigits(uint32_t x)
{
  uint32_t n = 1;

  while(1)
  {
    if (uint32_pow(10, n) > x) return n;
    n++;
  }
}

