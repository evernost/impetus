// ============================================================================
// Project       : impetus
// Module name   : -
// File type     : C
// File name     : solver.c
// Purpose       :
// Author        : QuBi (nitrogenium@outlook.fr)
// Creation date : Monday, 14 September 2026
// ----------------------------------------------------------------------------
// Best viewed with space indentation (2 spaces)
// ============================================================================

// ============================================================================
// DESCRIPTION
// ============================================================================
// TODO!



// ============================================================================
// INCLUDES
// ============================================================================
#include <inttypes.h>   // For portable support to print 'uint32_t' numbers
#include <stdio.h>
#include <stdlib.h>     // For 'stdtoul'
#include <string.h>
#include <time.h>       // For random numbers

// Max number of digits that can be manipulated.
// It can certainly be extended up to 65535 without too much trouble.
// Beyond that, there will be issues (size is stored in 'uint16_t')
#define MAX_DIGITS 210


#define STYLE       "\033[4m"
#define STYLE_RESET "\033[0m"



// Define a "MegaNum(C)" type for big fat numbers
typedef struct
{
  uint8_t digits[MAX_DIGITS];
  uint16_t len;
} MegaNum_t;



// ============================================================================
// PROTOTYPES
// ============================================================================
int parseArgs(int argc, char *argv[], MegaNum_t* x);

int megaNum_updateLen(MegaNum_t* x);
int megaNum_randInit(MegaNum_t* x, uint16_t size);
int megaNum_mult(MegaNum_t* a, MegaNum_t* b, MegaNum_t* y);

int _multMakeTable(MegaNum_t table[9], MegaNum_t* x);
int _multAccumulate(MegaNum_t* accu, MegaNum_t a_multTable[9], MegaNum_t* b, uint16_t b_digitPtr);

int roulette(MegaNum_t* x);

uint32_t score(MegaNum_t x, MegaNum_t target);
uint16_t _scoreDigit(uint8_t x, uint8_t y);
void print(MegaNum_t x);



// ============================================================================
// ENTRY POINT
// ============================================================================
int main(int argc, char *argv[])
{
  int ret;
  uint32_t bestScore = 65535;
  MegaNum_t a = {.digits = {0}, .len = 1};
  MegaNum_t b = {.digits = {0}, .len = 1};
  MegaNum_t prod = {.digits = {0}, .len = 1};

  // Default target number
  // Endianness: just type it like you would type it naturally.
  // Just ignore the 'len' property, the 'megaNum_updateLen()' function
  // will do it for you.
  // EXAMPLE: below the number 100000980001501
  MegaNum_t target = {.digits = {1,0,0,0,0,0,9,8,0,0,0,1,5,0,1}, .len = 0};
  megaNum_updateLen(&target);

  // --------------------------------------------------------------------------
  // PARSE THE INPUT
  // --------------------------------------------------------------------------
  ret = parseArgs(argc, argv, &target);
  if (ret != EXIT_SUCCESS) return ret;

  // --------------------------------------------------------------------------
  // SEARCH INIT
  // --------------------------------------------------------------------------
  srand((unsigned)time(NULL));
  megaNum_randInit(&a, target.len/2 + 2);
  megaNum_randInit(&b, target.len/2 + 2);

  // --------------------------------------------------------------------------
  // SEARCH LOOP
  // --------------------------------------------------------------------------
  for(uint32_t i = 0; i < 100000000; i++)
  {
    uint32_t scoreCurr;

    //megaNum_randInit(&a, target.len/2);
    //megaNum_randInit(&b, target.len/2);
    roulette(&a);
    roulette(&b);

    for(uint16_t z = 0; z < MAX_DIGITS; z++){prod.digits[z] = 0;}

    megaNum_mult(&a, &b, &prod);
    scoreCurr = score(prod, target);

    if (scoreCurr < bestScore)
    {
      bestScore = scoreCurr;
      printf("- a       = "); print(a); printf("\n");
      printf("- b       = "); print(b); printf("\n");
      printf("- prod    = "); print(prod); printf("\n");
      printf("- target  = "); print(target); printf("\n");
      printf("Best score = %" PRIu32 "\n", bestScore);
      printf("\n");
    }
  }

  // --------------------------------------------------------------------------
  // SEARCH RESULT
  // --------------------------------------------------------------------------
  print(prod);
  print(target);
  printf("Score = %" PRIu32 "\n", score(prod, target));


  return EXIT_SUCCESS;
}



int parseArgs(int argc, char *argv[], MegaNum_t* x)
{
  // No argument
  if (argc == 1)
  {
    printf("[DEBUG] No input specified, using the default target.\n");
  }

  // The target number is explicitely specified
  else if (argc == 2)
  {
    // Read the input and fill the target
    for(uint16_t i = 0; i < MAX_DIGITS; i++)
    {
      if (argv[1][i] == '\0')
      {
        printf("[DEBUG] Your input has %" PRIu32 " digit%s.\n", i, i > 1 ? "s" : "");
        x->len = i;
        break;
      }
      else if ((i == (MAX_DIGITS-1)) && (argv[1][i+1] != '\0'))
      {
        printf("[ERROR] Your input has %" PRIu32 " digit%s or more, which exceeds the max supported size (%" PRIu32 ").\n", i+2, i > 1 ? "s" : "", MAX_DIGITS);
        return EXIT_FAILURE;
      }
      else
      {
        x->digits[i] = argv[1][i] - '0';
      }
    }
  }

  // Anything else is invalid
  else if (argc > 2)
  {
    printf("Usage: %s [input_number]\n", argv[0]);
    return EXIT_FAILURE;
  }



  // Reverse input endianness.
  // For simplicity in the processing, we want index 0 to be the lowest significant digit
  // which is the opposite of a natural input.
  // Reverse is done using XOR swapping
  for(uint16_t i = 0; i < ((x->len)/2); i++)
  {
    x->digits[i]                 ^= x->digits[((x->len)-1) - i];
    x->digits[((x->len)-1) - i]  ^= x->digits[i];
    x->digits[i]                 ^= x->digits[((x->len)-1) - i];
  }

  return EXIT_SUCCESS;
}



int megaNum_updateLen(MegaNum_t* x)
{
  uint16_t i = MAX_DIGITS-1;

  while(x->digits[i] == 0)
  {
    if (i == 0)
    {
      x->len = 1;
      return EXIT_SUCCESS;
    }
    i--;
  }

  x->len = i+1;
  return EXIT_SUCCESS;
}



int megaNum_randInit(MegaNum_t* x, uint16_t size)
{
  for(uint16_t i = 0; i < size; i++)
  {
    x->digits[i] = (uint8_t)(rand() % 10);
  }
  megaNum_updateLen(x);

  return EXIT_SUCCESS;
}



int megaNum_mult(MegaNum_t* a, MegaNum_t* b, MegaNum_t* y)
{
  int ret;
  MegaNum_t a_multTable[9];

  // Generate the list of multiples of 'a'
  _multMakeTable(a_multTable, a);
  
  // Do the actual multiplication
  for(uint16_t i = 0; i < (b->len); i++)
  {
    ret = _multAccumulate(y, a_multTable, b, i);

    if (ret != EXIT_SUCCESS) return ret;
  }

  return EXIT_SUCCESS;
}



int _multMakeTable(MegaNum_t table[9], MegaNum_t* x)
{
  // Multiply 'x' by integers from 1 to 9
  for(uint8_t i = 1; i < 10; i++)
  {
    uint8_t prod;
    uint8_t carry = 0;

    // Create a local alias for the current multiple.
    // NOTE: the array stores only 9 multiples because multiples
    // of '0' are trivial.
    MegaNum_t *elem = &table[i-1];

    // By default, assume that the i-th multiple of 'x' has
    // as many digits as 'x'.
    elem->len = x->len;

    for(uint16_t d = 0; d < (x->len); d++)
    {
      prod = ((x->digits[d])*i) + carry;

      elem->digits[d] = prod % 10;
      carry           = prod / 10;
    }
    
    if (carry != 0)
    {
      elem->digits[x->len] = carry;
      elem->len++;
    }
  }

  return EXIT_SUCCESS;
}



int _multAccumulate(MegaNum_t* accu, MegaNum_t a_multTable[9], MegaNum_t* b, uint16_t b_digitPtr)
{
  uint16_t shift;
  uint8_t index;
  uint8_t multiplier;
  uint8_t carry = 0;
  uint8_t sum;

  if (b_digitPtr >= (b->len))
  {
    printf("[ERROR] _multAccumulate(): 'b_digitPtr' points outside the length of 'b'. Possible internal error, sorry for that.\n");
    return EXIT_FAILURE;
  }

  // Multiply by 0: nothing to do
  if (b->digits[b_digitPtr] == 0)
  {
    return EXIT_SUCCESS;
  }

  // Aliases
  shift       = b_digitPtr;
  multiplier  = b->digits[b_digitPtr];
  index       = multiplier-1;

  // Add the product a*b[i] to the accumulator at the right place (shifted 'i' times)
  for(uint16_t d = 0; d < (a_multTable[index].len); d++)
  {
    sum = accu->digits[shift + d] + a_multTable[index].digits[d] + carry;

    accu->digits[shift + d] = sum % 10;
    carry                   = sum / 10;
  }

  if (carry != 0)
  {
    accu->digits[shift + a_multTable[multiplier-1].len]++;
    accu->len = shift + a_multTable[multiplier-1].len + 1;
  }
  else
  {
    accu->len = shift + a_multTable[multiplier-1].len;
  }

  return EXIT_SUCCESS;
}



uint32_t score(MegaNum_t x, MegaNum_t target)
{
  uint32_t out = 0;

  if (x.len > target.len) out += 100;

  for (uint16_t i = 0; i < target.len; i++)
  {
    if (i == (target.len-1))
    {
      out += (10*_scoreDigit(x.digits[i], target.digits[i]));
    }
    else
    {
      out += _scoreDigit(x.digits[i], target.digits[i]);
    }

  }

  return out;
}



uint16_t _scoreDigit(uint8_t x, uint8_t y)
{
  if (x == y) return 0;

  if (x > y)
  {
    return (((x-y) > 5) ? (10-(x-y)) : (x-y));
  }
  else
  {
    return (((y-x) > 5) ? (10-(y-x)) : (y-x));
  }
}



void print(MegaNum_t x)
{
  // for(uint16_t i = x.len; i-- > 0; )
  // {
  //   printf("%d%s", x.digits[i], i == 0 ? "\n" : "");
  // }

  for(uint16_t i = x.len; i-- > 0; )
  {
    printf("%d", x.digits[i]);
  }

  //printf(" (len = %d)\n", x.len);
}



int roulette(MegaNum_t* x)
{
  uint16_t randLoc;
  //uint16_t randVal;

  randLoc = (uint16_t)(rand() % (x->len));
  //randVal = (uint8_t)(rand() % 10);

  x->digits[randLoc] = (x->digits[randLoc] < 9) ? (x->digits[randLoc] + 1) : 0;

  //x->digits[randLoc] = randVal;

  return EXIT_SUCCESS;
}





