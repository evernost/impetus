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

// Max number of digits supported for the target
// It can certainly be extended up to 65535 without
// too much issue.
// Beyond that, there will be issues (size is stored in 'uint16_t')
#define MAX_DIGITS 512


#define STYLE       "\033[4m"
#define STYLE_RESET "\033[0m"



// ============================================================================
// PROTOTYPES
// ============================================================================
int parseArgs(int argc, char *argv[], uint8_t* target, uint16_t target_len);
void initMultiplicand(uint8_t* x, uint16_t target_len);

void mult(uint8_t* a, uint16_t len_a, uint8_t* b, uint16_t len_b, uint8_t* y);
void _multTable(uint8_t table[10][MAX_DIGITS], uint8_t* a, uint16_t len_a);
void _multAccumulate(uint8_t* acc, uint8_t table[10][MAX_DIGITS], uint16_t tableLen, uint8_t index, uint16_t shift);

uint16_t getLen(uint8_t* x);
void score(uint8_t* x, uint8_t* y, uint16_t target_len);
void print(uint8_t* x, uint16_t len_x);



// ============================================================================
// ENTRY POINT
// ============================================================================
int main(int argc, char *argv[])
{
  int ret;
  uint8_t a[MAX_DIGITS] = {0};
  uint8_t b[MAX_DIGITS] = {0};
  uint8_t res[MAX_DIGITS] = {0};

  // Default target number
  // Endianness: just type it like you would type it naturally.
  uint8_t target[MAX_DIGITS] = {1,0,0,0,0,0,9,8,0,0,0,1,5,0,1};
  uint32_t target_len = 15;

  // --------------------------------------------------------------------------
  // PARSE THE INPUT NUMBER
  // --------------------------------------------------------------------------
  ret = parseArgs(argc, argv, target, target_len);
  if (ret != EXIT_SUCCESS) return ret;

  // Reverse input endianness: index 0 is the lowest significant digit.
  // Reverse is done using XOR swapping
  for(uint32_t i = 0; i < (target_len/2); i++)
  {
    target[i]                   ^= target[(target_len-1) - i];
    target[(target_len-1) - i]  ^= target[i];
    target[i]                   ^= target[(target_len-1) - i];
  }

  // --------------------------------------------------------------------------
  // SEARCH INIT
  // --------------------------------------------------------------------------
  srand((unsigned)time(NULL));
  initMultiplicand(a, target_len);
  initMultiplicand(b, target_len);

  // --------------------------------------------------------------------------
  // SEARCH MAIN LOOP
  // --------------------------------------------------------------------------
  print(a, 32);
  print(b, 32);
  printf("%d\n", getLen(a));
  mult(a, 15, b, 15, res);

  // --------------------------------------------------------------------------
  // SEARCH RESULT
  // --------------------------------------------------------------------------
  print(res, 32);
  print(target, 32);




  return EXIT_SUCCESS;
}



int parseArgs(int argc, char *argv[], uint8_t* target, uint16_t target_len)
{
  if (argc == 1)
  {
    printf("[DEBUG] No input specified, using the default target.\n");
    return EXIT_SUCCESS;
  }
  else if (argc == 2)
  {
    for(uint32_t i = 0; i < MAX_DIGITS; i++)
    {
      if (argv[1][i] == '\0')
      {
        printf("[DEBUG] Your input has %" PRIu32 " digit%s.\n", i, i > 1 ? "s" : "");
        target_len = i;
        break;
      }
      else if ((i == (MAX_DIGITS-1)) && (argv[1][i+1] != '\0'))
      {
        printf("[ERROR] Your input has %" PRIu32 " digit%s or more, which exceeds the max supported size (%" PRIu32 ").\n", i+2, i > 1 ? "s" : "", MAX_DIGITS);
        return EXIT_FAILURE;
      }
      else
      {
        target[i] = argv[1][i] - '0';
      }
    }

    // Fill the leading digits with zeros
    for(uint32_t i = target_len; i < MAX_DIGITS; i++)
    {
      target[i] = 0;
    }
  }
  else if (argc > 2)
  {
    printf("Usage: %s [input_number]\n", argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}



void mult(uint8_t* a, uint16_t len_a, uint8_t* b, uint16_t len_b, uint8_t* y)
{
  uint8_t table[10][MAX_DIGITS] = {0};

  // Generate the list of multiples of 'a'
  _multTable(table, a, len_a);
  
  for(uint16_t i = 0; i < len_b; i++)
  {
    if (b[i] != 0) _multAccumulate(y, table, len_a, b[i], i);
  }
}



void _multTable(uint8_t table[10][MAX_DIGITS], uint8_t* a, uint16_t len_a)
{
  // Multiply 'a' by integers from 1 to 9
  for(uint8_t i = 1; i < 10; i++)
  {
    uint8_t prod;
    uint8_t carry = 0;

    for(uint16_t j = 0; j < len_a; j++)
    {
      prod = (a[j]*i) + carry;
      table[i][j] = prod % 10;
      carry = prod / 10;
    }
    
    table[i][len_a] = carry;
    
    // printf("mult = %d\n", i);
    // print(table[i], len_a+1);
  }
}



void _multAccumulate(uint8_t* acc, uint8_t table[10][MAX_DIGITS], uint16_t table_len, uint8_t index, uint16_t shift)
{
  uint8_t carry = 0;
  uint8_t sum;

  if ((shift + table_len) >= MAX_DIGITS)
  {
    printf("[ERROR] _multAccumulate(): unhandled exception (input possibly too big)\n");
    exit(EXIT_FAILURE);
  }

  // NOTE: the '+1' in 'table_len+1' is to account for a possible carry.
  for(uint16_t i = 0; i < (table_len + 1); i++)
  {
    sum = acc[shift + i] + table[index][i] + carry;

    acc[shift + i] = sum % 10;
    carry = sum / 10;
  }
}



void initMultiplicand(uint8_t* x, uint16_t target_len)
{
  for(uint16_t i = 0; i < (target_len/2); i++)
  {
    x[i] = (uint8_t)(rand() % 10);
  }
}



uint16_t getLen(uint8_t* x)
{
  uint16_t i = MAX_DIGITS-1;

  while(x[i] == 0)
  {
    if (i == 0) return 0;
    i--;
  }

  return (i+1);
}



void score(uint8_t* x, uint8_t* y, uint16_t target_len)
{

}



void print(uint8_t* x, uint16_t len_x)
{
  for(uint16_t i = len_x; i-- > 0; )
  {
    printf("%d%s", x[i], i == 0 ? "\n" : "");
  }
}









