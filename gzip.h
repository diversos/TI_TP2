/* Author: Rui Pedro Paiva
Teoria da Informação, LEI, 2007/2008
*
*Adaptado por:
*André Monteiro do Rosário Almeida, 2013152895
*Mariana da Cruz Cunha, 2014214727
*Sérgio José Marschall, 2014197357
*
*/

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
char getBits(int needBits);
char readBlockFormat(int type);
int indexFromTree(HuffmanTree *Huffman_tree);
void LenCode_HCLEN(int dim, int *CodeLen_HCLEN);
char** ConverterHuffman(HuffmanTree* Huffman_tree, int *CodeLen_HCLEN, int dim);
void descompactacao(HuffmanTree *Huffman_treeLIT, HuffmanTree *Huffman_treeDIST, char *nome);
char *int2Binary(int number, int lenCode);
int* LenCode(int dim, HuffmanTree *Huffman_tree);
