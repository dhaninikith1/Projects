/**********************************************/
/* Hani Nikith Devarapalli                    */
/* 12-9-16                                    */
/* CS-241 Section 001                         */
/* Lab 9                                      */
/* Encoding or decoding a file with a         */
/* compression algorithm using huffman codes. */
/* Methods are called via additional files    */
/* called huffencode.c and huffdecode.c       */
/**********************************************/

#include <stdio.h>
#include "huffman.h"
#include <ctype.h>

int main(int argc, char** argv)
{
  char* infile;
  char* outfile;
  FILE* in;
  FILE* out;

  /* Setting all values to 0 in this array */
  /* unsigned long freqArray[256] = {0}; */
  /* struct Code codes[256]; */
  /* struct Node* node; */
  /* int numChars = 0; */

  if(argc != 3) 
  {
    printf("wrong number of args\n");
    return 1;
  }

  infile = argv[1];
  outfile = argv[2];

  in = fopen(infile, "rb");
  if(in == NULL)
  {
    printf("couldn't open %s for reading\n", infile);
    return 2;
  }

  out = fopen(outfile, "wb");
  if(out == NULL)
  {
    printf("couldn't open %s for writing\n", outfile);
    return 3;
  }

  /* Getting frequencies of characters and printing out the table */
  /* numChars = getFrequencies(freqArray, in); */

  /* debug... */
  /* fprintf(out, "Number of characters: %d", numChars); */

  /* This node serves as a list */
  /* node = insertFrequencies(freqArray); */

  /* */
  /* node = makeHuffmanTree(node); */


  encodeFile(in, out);

  fclose(in);
  fclose(out);

  return 0;
}
