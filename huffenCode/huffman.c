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


#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* encodeFile */
/* "Main" for huffencode.c */
/* Runs all the methods to encode a file with huffman encoding */
void encodeFile(FILE* in, FILE* out)
{
  unsigned long freqArray[256] = {0};
  struct Node* node = NULL;
  struct Code codes[256] = {{0}};
  unsigned char* code;
  unsigned long numChars;

  code = calloc(32, sizeof(char));
  
  /* Getting the number of total characters in the file */
  numChars = getFrequencies(freqArray, in);
  
  /* At this point node is strictly a linked list, node is the head */  
  node = insertFrequencies(freqArray);

  /* At this point node is a binary tree, node is the head of the tree */
  node = makeHuffmanTree(node);

  /* Populating the array of codes */
  populateCodes(node, codes, code, 0);

  /* Write out header to stdout */
  writeHeader(freqArray, numChars, codes);
  
  /* Writing the encoded file */
  encodeToFile(freqArray, numChars, codes, in, out);

  /* Freeing the memory of the tree nodes and Code structs */
  freeTree(node);
  freeCode(codes);
}


/* decodeFile */
/* "Main" for huffdecode.c */
/* Runs all the methods to decode a file with huffman encoding */
void decodeFile(FILE* in, FILE* out)
{
  unsigned char c;
  unsigned int numSymbols;
  unsigned char mask;
  unsigned long freqArray[256] = {0};
  unsigned long numChars;
  struct Node* root = NULL;
  struct Node* currentNode = NULL;

  /* Read in numSymbols first... */
  readHeader(freqArray, &numSymbols, &numChars, in);

  /* Making the tree */
  root = insertFrequencies(freqArray);
  root = makeHuffmanTree(root);
  
  /* Place holder for where we are in the tree */
  currentNode = root;

  c = fgetc(in);

  /* 1 in first position, 1000 0000 */
  mask = 0x80;

  /* Beginning reading in actual data and traversing the tree */
  while(numChars)
  {
    /* If the current node is at a leaf, then write a character and get a new one */
    if(currentNode->left == NULL && currentNode->right == NULL)
    {
      fwrite(&currentNode->sym, sizeof(char), 1, out);
      numChars--;
      currentNode = root;
    }

    /* Read in a bit from file */
    if(c & mask)
    {
      currentNode = currentNode->right;
    }
    else
    {
      currentNode = currentNode->left;
    }

    mask >>= 1;
    if(mask == 0)
    {
      mask = 0x80;
      c = fgetc(in);
    }
  }
  /* Freeing memory */
  freeTree(root);
}


/* readHeader: */
/* Reading the first sections of a huffman decoded file */
/* Gets the number of symbols, symbol/frequency pairs, and total num of symbols */
void readHeader(unsigned long freqArray[256], unsigned int *numSymbols, 
                unsigned long *numChars, FILE* in)
{
  unsigned char symbol;
  unsigned int i;
  unsigned long frequency;

  *numSymbols = fgetc(in);

  if(*numSymbols == 0)
  {
    *numSymbols = 256;
  }

  for(i = 0; i < *numSymbols; ++i)
  {
    symbol = fgetc(in);
    fread(&frequency, sizeof(unsigned long), 1, in);
    freqArray[symbol] = frequency;
  }

  fread(numChars, sizeof(unsigned long), 1, in);

}


/* getFrequencies: */
/* Gets the frequency of each character in a given file and fills an array */
/* Returns an int that represents the total number of characters in thefile */
int getFrequencies(unsigned long freqArray[256], FILE* in)
{
  int c, i;
  unsigned long numOfChars = 0;
  c = i = 0; 

  while((c = getc(in)) != EOF)
  {
    /* Assigning frequencies of each character into the array */
    freqArray[c]++;
  }
  
  for(i = 0; i < 256; ++i)
  {
    /* Adding each array value to the total num of characters */
    numOfChars += freqArray[i];
  }

  return numOfChars;
}


/* insertFrequencies: */
/* Inserts all of the frequencies into a linked list structure */
/* Returns the head of the list */
struct Node* insertFrequencies(unsigned long freqArray[256])
{ 
  int i;

  struct Node* node = NULL;

  for(i = 0; i < 256; ++i)
  {
    /* If the characters occurs then insert a new node into the list */
    if(freqArray[i] > 0)
    {
      node = insertSorted(node, createNode(i, freqArray[i]));
    }
  }

  return node;

}


/* makeHuffmanTree: */
/* Inserts all items from the linked list into a binary tree structure */
/* This also creates parent nodes for each pair of two and then pairs those parents */
/* Retrns the root of the binary tree */
struct Node* makeHuffmanTree(struct Node* head)
{
  unsigned long parentsFrequency;
  unsigned char parentsSymbol;
  struct Node* parent;
   
  /* Take two nodes and create a tree */
  /* Loop until head->next is null */
  while(head->next != NULL)
  {
    parentsFrequency = head->freq + head->next->freq;
    parentsSymbol = head->next->sym;
    parent = createNode(parentsSymbol, parentsFrequency);
    
    /* Assigning the children to the new parent node we created */
    parent->left = head;
    parent->right = head->next;

    head = head->next->next;
    head = insertSorted(head, parent); 
  }

  /* Returning the root of the tree */
  return parent;
}


/* printCode: */
/* Prints a huffman code for a given index of a character */
/* Prints 1's and 0's corresponding to the code */
void printCode(struct Code codes[256], unsigned int i)
{
  int  j;
  
  /* If the length of the code index is more than 0 then it exists */
  if(codes[i].len > 0)
  {
  /* This code is a bit treacherous */
  /* Looping over 256 bits minus the length */
  for(j = 255; j >= 256 - codes[i].len; --j)
  {
    /* abs(31 - j/8) uses integer division to get the correct index of the byte */
    /* 1 << (7 - j * 8) bit shifts a 1 in the place of where our counter bit is */
    if(codes[i].code[abs(31 - j/8)] & (1 << (7 - j % 8)))
    {
      printf("1");
    }
    else
    {
      printf("0");
    }
  }
  printf("\n");
  }
}


/* writeHeader: */
/* Prints or writes the human readable header for Symbol Freq and Code */
void writeHeader(unsigned long freqArray[256], unsigned long numChars, struct Code codes[256])
{
  int i;

  /* Header print */
  printf("Symbol\tFreq\tCode\n");
  for(i = 0; i < 256; ++i)
  {
    if(freqArray[i] > 0 && i >= 33 && i <= 126)
    {
      printf("%c\t%lu\t", i, freqArray[i]);
      printCode(codes, i);
    }
    else if(freqArray[i] > 0)
    {
      printf("=%d\t%lu\t", i, freqArray[i]);
      printCode(codes, i);
    }
  }

  printf("Total chars = %lu\n", numChars);
}


/* encodeToFile: */
/* Writes the legend/header for the file as well as the data */
/* Edits the file in place */
void encodeToFile(unsigned long freqArray[256], unsigned long numChars, struct Code codes[256], FILE* in, FILE* out)
{
  unsigned char numSymbols, assembledChar, c;
  unsigned int i, fileBitPosition;
  int j;
  unsigned char* tempCode;

	i = j = fileBitPosition = 0;
  numSymbols = assembledChar = c = 0;
  
  for(i = 0; i < 256; ++i)
  {
    if(freqArray[i] > 0)
    {
      numSymbols++;
    }
  }
  
  /* Writing the amount of symbols to expect */
  fwrite(&numSymbols, sizeof(char), 1, out);

	/* Write a stream of bits to a file for legend in the format: */
	/* Symbol Frequency, starting at ascii 0 and ending at ascii 255 */
	for(i = 0; i < 256; ++i)
	{
		/* Write a symbol and frequency pair */
		if(freqArray[i] > 0)
		{
      fwrite(&i, sizeof(unsigned char), 1, out);
      fwrite(&freqArray[i], sizeof(unsigned long), 1, out);
		}
	}

  /* Writing the total number of characters */
  fwrite(&numChars, sizeof(unsigned long), 1, out);

  /* Rewinding file to be read again */
  rewind(in);

  i = 0;
  /* Write data to file */
  while(1)
  {
    
    c = fgetc(in);
    if(feof(in))
    {
      break;
    }

    tempCode = codes[c].code;
    /* Processing a single character/huffman code */
    for(j = 0; j < codes[c].len; ++j)
    {
      /* Shifting one over */
      assembledChar <<= 1;

      /* If the bit value is 1 then write a 1 */
      if((tempCode[j/8] >> (j % 8)) & 1)
      {
        /* Adding a 1 */
        assembledChar |= 1;
      }
      i++;
      if(i == 8)
      {
        fwrite(&assembledChar, sizeof(char), 1, out);

        i = 0;
        assembledChar = 0;
      }
    }

  }
  
  /* If i has written at least 1 bit to assembledChar then pad with zeros */
  if(i != 0)
  {
    assembledChar <<= (8-i);
    fwrite(&assembledChar, sizeof(char), 1, out);
  }

}


/* freeTree: */
/* Frees all the memory that was allocated for each node */
void freeTree(struct Node* root)
{
  if(root == NULL)
  {
    return;
  }
  freeTree(root->left);
  freeTree(root->right);
  free(root);
}


/* freeCode: */
/* Frees the Code structs that are used for the huffman codes */
void freeCode(struct Code codes[256])
{
  int i;
  for(i = 0; i < 256; ++i)
  {
    if(codes[i].len > 0)
    {
      free(codes[i].code);
    }
  }
}


/* populateCodes: */
/* Filling the codes array of structs with each leaf in the binary tree structure */
/* A counter corresponds to the length of bits in the code */
void populateCodes(struct Node* root, struct Code codes[256], unsigned char* code, int counter)
{

  /* We've reached a legitimate character */
  if(root->left == NULL && root->right == NULL)
  {
    codes[root->sym].code = code;
    codes[root->sym].len = counter;
  }

  if(root->left)
  {
    /* append a 0... */    
    unsigned char* codeCopy = malloc(32 * sizeof(char));
    memcpy(codeCopy, code, 32 *sizeof(char));
    populateCodes(root->left, codes, codeCopy, counter + 1);
  }

  if(root->right)
  {
    /* append a 1... */
    returnCode(code, 1, counter);
    populateCodes(root->right, codes, code, counter + 1);
  }
}


/* returnCode: */
/* Edits the char* code in place based on the num given leftOrRight and places it at */
/* bit corresponded to by the counter */
void returnCode(unsigned char* code, unsigned int leftOrRight, unsigned int counter)
{
  int bitPosition, bytePosition;
  bitPosition = bytePosition = 0;
  bitPosition = counter % 8;
  bytePosition = counter / 8;
  code[bytePosition] |= (leftOrRight << bitPosition);

}


/* compareNodes: */
/* Compares nodes for higher priority */
/* Returns a 1 if node1 is higher priority than node2 based on frequency first then symbol */
int compareNodes(struct Node* node1, struct Node* node2)
{
  if(node1->freq > node2->freq)
  {
    return 1;
  }
  else if(node1->freq < node2->freq)
  {
    return 0;
  }
  else
  {
    return (node1->sym < node2->sym);
  }
}


/* insertSorted: */
/* Inserts a given node into a linked list in sorted order */
/* Returns the head of the list */
struct Node* insertSorted(struct Node* head, struct Node* node)
{
  if(head == NULL || compareNodes(head, node))
  {   
    node->next = head;
    return node;
  }
  else
  {
    head->next = insertSorted(head->next, node);
    return head;
  }

}


/* createNode: */
/* Creates a new node with symbol and frequency data given */
/* Returns the new node that was created */
struct Node* createNode(unsigned char symbol, unsigned long frequency)
{
  struct Node* node = malloc(sizeof(struct Node));
  node->left = NULL;
  node->right = NULL;
  node->next = NULL;
  node->sym = symbol;
  node->freq = frequency;
  return node;
}

