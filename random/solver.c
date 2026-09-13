#include <inttypes.h>   // For portable support to print 'uint32_t' numbers
#include <stdio.h>
#include <stdlib.h>     // For 'stdtoul'
#include <string.h>


#define STYLE       "\033[4m"
#define STYLE_RESET "\033[0m"

#define MAX_DIGITS 512



void mult(uint8_t* a, uint16_t len_a, uint8_t* b, uint16_t len_b, uint8_t* y);
void _multAdd(uint8_t* acc, uint8_t* table, uint16_t index, uint16_t shift);
void print(uint8_t* a, uint16_t len_a);



int main(int argc, char *argv[])
{
  uint8_t a[MAX_DIGITS] = {0};
  uint8_t b[MAX_DIGITS] = {0};
  uint8_t res[MAX_DIGITS] = {0};

  uint8_t target[MAX_DIGITS] = {1,0,0,0,0,0,9,8,0,0,0,1,5,0,1};
  uint32_t inputSize = 15;



  // --------------------------------------------------------------------------
  // PARSE ARGUMENTS
  // --------------------------------------------------------------------------
  if (argc == 2)
  {
    for(uint32_t i = 0; i < MAX_DIGITS; i++)
    {
      if (argv[1][i] == '\0')
      {
        printf("[DEBUG] Your input has %" PRIu32 " digit%s.\n", i, i > 1 ? "s" : "");
        inputSize = i;
        break;
      }
      else if ((i == (MAX_DIGITS-1)) && (argv[1][i+1] != '\0'))
      {
        printf("[ERROR] Your input has %" PRIu32 " digit%s or more, which exceeds the max supported size (%" PRIu32 ").\n", i+2, i > 1 ? "s" : "", MAX_DIGITS);
        return 1;
      }
      else
      {
        target[i] = argv[1][i] - '0';
      }
    }
  }
  else if (argc > 2)
  {
    printf("Usage: %s [input_number]\n", argv[0]);
    return 1;
  }

  // Reverse input endianness: index 0 is the lowest significant digit.
  for(uint32_t i = 0; i < (inputSize/2); i++)
  {
    target[i]                 ^= target[(inputSize-1) - i];
    target[(inputSize-1) - i] ^= target[i];
    target[i]                 ^= target[(inputSize-1) - i];
  }

  mult(a, 13, a, 13, res);

  // printf("[DEBUG] New number: ");
  // for(uint32_t i = 0; i < inputSize; i++)
  // {
  //   printf("%d", target[i]);
  // }
  // printf("\n");




  return 0;
}




void mult(uint8_t* a, uint16_t len_a, uint8_t* b, uint16_t len_b, uint8_t* y)
{
  uint8_t table[10][MAX_DIGITS] = {0};

  _multTable(table, a, len_a);
  
  for(uint16_t i = 0; i < len_b; i++)
  {
    if (b[i] != 0) _multAccumulate(y, table, b[i], i);
  }

}



void _multTable(uint8_t* table, uint8_t* a, uint16_t len_a)
{
  for(uint16_t i = 1; i < 10; i++)
  {
    uint8_t prod;
    uint8_t carry = 0;

    for(uint16_t j = 0; j < len_a; j++)
    {
      prod = (a[j]*i) + carry;
      table[i][j] = prod % 10;
      carry = prod / 10;
    }
    
    table[i][len_a] = (carry != 0) ? 1 : 0;
    
    // printf("mult = %d\n", i);
    // print(table[i], len_a+1);
  }
}


void _multAccumulate(uint8_t* acc, uint8_t* table, uint16_t index, uint16_t shift)
{

}



void print(uint8_t* a, uint16_t len_a)
{
  for(uint16_t i = len_a; i-- > 0; )
  {
    printf("%d %s", a[i], i == 0 ? "\n" : "");
  }
}








