#include <inttypes.h>   // For maximal portable support to print 'uint32_t' type
#include <stdio.h>
#include <stdlib.h>     // For 'stdtoul'
#include <string.h>


#define STYLE       "\033[4m"
#define STYLE_RESET "\033[0m"


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
  char buf[16];



  // --------------------------------------------------------------------------
  // PARSE ARGUMENTS
  // --------------------------------------------------------------------------
  if (argc != 3)
  {
    printf("Usage: %s <nDigits> <ending>\n", argv[0]);
    return 1;
  }

  printf("\n");
  nDigits = (uint32_t)strtoul(argv[1], NULL, 0);
  printf("- allowed number of digits: %" PRIu32 "\n", nDigits);
  if (nDigits == 0) return 0;

  ending = (uint32_t)strtoul(argv[2], NULL, 0);
  printf("- target ending: %" PRIu32 "\n", ending);

  mod = uint32_pow(10, get_nDigits(ending));
  printf("Required modulo: mod %" PRIu32 "\n", mod);
  printf("\n");



  // --------------------------------------------------------------------------
  // MAIN LOOP
  // --------------------------------------------------------------------------
  for(a = 0; a < uint32_pow(10, nDigits); a++)
  {
    for(b = a; b < uint32_pow(10, nDigits); b++)
    {
      if ((a*b) % mod == ending)
      {
        printf("- %" PRIu32 " x %" PRIu32 " =", a, b);

        snprintf(buf, sizeof(buf), "%" PRIu32, a*b);

        {
          uint32_t n = get_nDigits(ending);
          uint32_t len = strlen(buf);
          if (n > len) n = len;
          int split = len - n;
          printf("%.*s", split, buf);
          printf(STYLE "%s" STYLE_RESET "\n", buf + split);
        }

        count++;
      }
    }
  }



  // --------------------------------------------------------------------------
  // SUMMARY
  // --------------------------------------------------------------------------
  printf("\n");
  printf("%" PRIu32 " solutions found.\n", count);
  printf("\n");
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

