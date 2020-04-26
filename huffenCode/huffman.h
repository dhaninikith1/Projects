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

#ifndef HUFFMAN_H
#define HUFFMAN_H

/* Including stdio so we'll know about FILE type */
#include <stdio.h>

/* BST and linked list combined */
struct Node
{
  struct Node* left;
  struct Node* right;
  struct Node* next;
  unsigned char sym;
  unsigned long freq;
};

/* Custom data type to hold huffman codes, will hold 32x8 bits, 256 */
/* Holding length of the character so we can differentiate 001 and 0001 for codes */
struct Code
{
  unsigned char* code;
  int len;
};

/* The following 2 functions are used in my huffencode and huffdecode
   programs. You'll need to write them if you want to use my code.  */

/**************************************************************/
/* Huffman encode a file.                                     */
/*     Also writes freq/code table to standard output         */
/* in -- File to encode.                                      */
/*       May be binary, so don't assume printable characters. */
/* out -- File where encoded data will be written.            */
/**************************************************************/
void encodeFile(FILE* in, FILE* out);

/***************************************************/
/* Decode a Huffman encoded file.                  */
/* in -- File to decode.                           */
/* out -- File where decoded data will be written. */
/***************************************************/
void decodeFile(FILE* in, FILE* out);

/* Decode helper method */
void readHeader(unsigned long freqArray[256], unsigned int *numSymbols,
                unsigned long *numChars, FILE* in); 

/* Populates the frequency array based on the file coming in */
int getFrequencies(unsigned long freqArray[256], FILE* in);

/* Writing the header including the frequencies, codes, and chars */
void writeHeader(unsigned long freqArray[256], unsigned long numChars, struct Code codes[256]);
void encodeToFile(unsigned long freqArray[256], unsigned long numChars, struct Code codes[256], FILE* in, FILE* out);

/* List methods */
struct Node* insertFrequencies(unsigned long freqArray[256]);
struct Node* insertSorted(struct Node* head, struct Node* node);
struct Node* createNode(unsigned char symbol, unsigned long frequency);
int compareNodes(struct Node* node1, struct Node* node2);


/* Tree methods */
struct Node* makeHuffmanTree(struct Node* head);
void freeTree(struct Node* root);
void freeCode(struct Code codes[256]);


void populateCodes(struct Node* root, struct Code codes[256], unsigned char* code, int counter);
void returnCode(unsigned char* code, unsigned int leftOrRight, unsigned int counter);


#endif
