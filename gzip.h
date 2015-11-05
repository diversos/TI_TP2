/* Author: Rui Pedro Paiva
Teoria da Informação, LEI, 2007/2008*/

#include <stdio.h>

// Gzip header variables
typedef struct header
{
	//elementos fixos
	unsigned char ID1, ID2, CM, XFL, OS;
	unsigned long MTIME;
	unsigned char FLG_FTEXT, FLG_FHCRC, FLG_FEXTRA, FLG_FNAME, FLG_FCOMMENT;   //bits 0, 1, 2, 3 e 4, respectivamente (restantes 3: reservados)

	// FLG_FTEXT --> ignorado deliberadamente (tipicamente igual a 0)
	//se FLG_FEXTRA == 1
	unsigned char xlen;
	unsigned char *extraField;

	//se FLG_FNAME == 1
	char *fName;  //terminada por um byte a 0

	//se FLG_FCOMMENT == 1
	char *fComment; //terminada por um byte a 0

	//se FLG_HCRC == 1
	unsigned char *HCRC;
} gzipHeader;



long getOrigFileSize();
int getHeader(gzipHeader *gzh);
int isDynamicHuffman(unsigned char rb);
void bits2String(char *strBits, unsigned char byte);
char readBlockFormat(int type);
int indexFromTree(HuffmanTree *Huffman_tree);
void LenCode_HCLEN(int dim, int *CodeLen_HCLEN);
void ConverterHuffman(HuffmanTree* Huffman_tree, int *CodeLen_HCLEN);
char *int2Binary(int number, int lenCode);
int* LenCode(int dim, HuffmanTree *Huffman_tree);
