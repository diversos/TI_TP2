/*Author: Rui Pedro Paiva
Teoria da Informação, LEI, 2007/2008
*
*Adaptado por:
*André Monteiro do Rosário Almeida, 2013152895
*Mariana da Cruz Cunha, 2014214727
*Sérgio José Marschall, 2014197357
*
*/

#include <cstdlib>
#include <unistd.h>
#include "huffman.h"
#include <sstream>

#include <cstring>

#include "gzip.h"

//Para o switch
#define n_HLIT 1
#define n_HDIST 2
#define n_HCLEN 3
#define bit_lenCode 4
#define extra_16 5
#define extra_17 6
#define extra_18 7
#define need_1 8

using namespace std;

char availBits = 0;
unsigned char byte;  //variável temporária para armazenar um byte lido directamente do ficheiro
unsigned int rb = 0;  //último byte lido (poderá ter mais que 8 bits, se tiverem sobrado alguns de leituras anteriores)
FILE *gzFile;  //ponteiro para o ficheiro a abrirs

//função principal, a qual gere todo o processo de descompactação
int main(int argc, char** argv)
{
	//--- Gzip file management variables
	long fileSize;
	long origFileSize;
	int numBlocks = 0;
	gzipHeader gzh;
	char needBits = 0;
    //criar árvore
    HuffmanTree* Huffman_tree_HCLEN = createHFTree();
    HuffmanTree* Huffman_tree_HLIT = createHFTree();
    HuffmanTree* Huffman_tree_HDIST = createHFTree();


	//--- obter ficheiro a descompactar
	char fileName[] = "FAQ.txt.gz";

	/*if (argc != 2)
	{
		printf("Linha de comando inválida!!!");
		return -1;
	}
	char * fileName = argv[1];*/

	//--- processar ficheiro
	gzFile = fopen(fileName, "r");
	fseek(gzFile, 0L, SEEK_END);
	fileSize = ftell(gzFile);
	fseek(gzFile, 0L, SEEK_SET);

	//ler tamanho do ficheiro original (acrescentar: e definir Vector com símbolos
	origFileSize = getOrigFileSize();


	//--- ler cabeçalho
	int erro = getHeader(&gzh);
	if (erro != 0)
	{
		printf ("Formato inválido!!!");
		return -1;
	}

	//--- Para todos os blocos encontrados
	char BFINAL;

	do
	{
		//--- ler o block header: primeiro byte depois do cabeçalho do ficheiro
		needBits = 3;
		if (availBits < needBits)
		{
			fread(&byte, 1, 1, gzFile);
			rb = (byte << availBits) | rb;
			availBits += 8;
		}

		//obter BFINAL
		//ver se é o último bloco
		BFINAL = rb & 0x01; //primeiro bit é o menos significativo
		printf("BFINAL = %d\n", BFINAL);
		rb = rb >> 1; //descartar o bit correspondente ao BFINAL
		availBits -=1;

		//analisar block header e ver se é huffman dinâmico
		if (!isDynamicHuffman(rb))  //ignorar bloco se não for Huffman dinâmico
			continue;
		rb = rb >> 2; //descartar os 2 bits correspondentes ao BTYPE
		availBits -= 2;

		//--- Se chegou aqui --> compactado com Huffman dinâmico --> descompactar
		//**************************************************
		//****** ADICIONAR PROGRAMA... *********************
		//**************************************************
		int *CodeLen_HCLEN = new int[19];	//comprimentos de código
		char** HuffmanCode_HCLEN;
		char** HuffmanCode_HLIT;
		char** HuffmanCode_HDIST;

        /*1ª semana - PONTO 1*/
        char HLIT = readBlockFormat(n_HLIT);
        int dim_HLIT = HLIT + 257;
        printf("HLIT = %d\n", HLIT);

        char HDIST = readBlockFormat(n_HDIST);
        int dim_HDIST = HDIST + 1;
        printf("HDIST = %d\n", HDIST);

        char HCLEN = readBlockFormat(n_HCLEN);
        int dim_HCLEN = HCLEN + 4;
        printf("HCLEN = %d\n", HCLEN);

        /*1ª semana - PONTO 2*/
        LenCode_HCLEN(dim_HCLEN, CodeLen_HCLEN);

        /*2ª semana - PONTO 3*/
        HuffmanCode_HCLEN = ConverterHuffman(Huffman_tree_HCLEN, CodeLen_HCLEN, 19);
printf("\nFIM\n");
        /*2ª semana - PONTO 4 e PONTO 5*/
        int *CodeLen_HLIT = new int[dim_HLIT];
        CodeLen_HLIT = LenCode(dim_HLIT,Huffman_tree_HCLEN);

        int *CodeLen_HDIST = new int[dim_HDIST];
        CodeLen_HDIST = LenCode(dim_HDIST,Huffman_tree_HCLEN);
        //printf("teste!! %d", CodeLen_HDIST[0]);
        /*3ª semana - PONTO 6*/
        printf("\n\n");
        HuffmanCode_HLIT = ConverterHuffman(Huffman_tree_HLIT,CodeLen_HLIT,dim_HLIT);
printf("\nFIM\n");
        HuffmanCode_HDIST = ConverterHuffman(Huffman_tree_HDIST,CodeLen_HDIST,dim_HDIST);

        for(int k=0;k<dim_HDIST;k++){
        printf("\n***** %s",HuffmanCode_HDIST[k]);
        }

		//actualizar número de blocos analisados
		numBlocks++;
	}while(BFINAL == 0);


	//terminações
	fclose(gzFile);
	printf("End: %d bloco(s) analisado(s).\n", numBlocks);


    //teste da função bits2String: RETIRAR antes de criar o executável final
	char str[9];
	bits2String(str, 0x03);
	printf("%s\n", str);

	//RETIRAR antes de criar o executável final
	system("PAUSE");
    return EXIT_SUCCESS;
}

/*1ª semana  - PONTO 1
*
*Função que lê o formato do bloco
*
*/
char readBlockFormat(int type){
    char format, needBits, readBits;
    switch (type){
    case n_HLIT:
        needBits = 5;
        readBits = 0x1F;
        break;
    case n_HDIST:
        needBits = 5;
        readBits = 0x1F;
        break;
    case n_HCLEN:
        needBits = 4;
        readBits = 0x0F;
        break;
    case bit_lenCode:
        needBits = 3;
        readBits = 0x07;
        break;
    case extra_16:
        needBits = 2;
        readBits = 0x03;
        break;
    case extra_17:
        needBits = 3;
        readBits = 0x07;
        break;
    case extra_18:
        needBits = 7;
        readBits = 0x7F;
        break;
    case need_1:
        needBits = 1;
        readBits = 0x01;
        break;
    }

    /*
     * Verifica se tem bits suficientes para ler
     * Caso não tenha vai buscar +1 byte (8bits) e adiciona aos que já tem
     */
    if (availBits < needBits){
        fread(&byte, 1, 1, gzFile);
        rb = (byte << availBits) | rb;
        availBits += 8;
    }

    // Guarda em format o número de bits que quer
    format = rb & ((1 << needBits) - 1);

    //Descarta o número de bits lidos
    rb = rb >> needBits;

    availBits -= needBits;
    return format;
}

/*
 * 1ª semana - PONTO 2
 *
 * Lê os comprimentos dos códigos do alfabeto, com base em HCLEN
 */
void LenCode_HCLEN(int dim, int *CodeLen_HCLEN){
	int ordem[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};	//ordem pela qual se lê do ficheiro
    int i;
    //inicializar a 0
    for(i=0; i<19; i++)
    	CodeLen_HCLEN[i] = 0;
    for(i=0; i<dim; i++){
        CodeLen_HCLEN[ordem[i]] = readBlockFormat(bit_lenCode);
    }
}

/*
 * 2ª semana - PONTO 3
 *
 * Converte para códigos huffman e adiciona-os à árvore
 */
char** ConverterHuffman(HuffmanTree* Huffman_tree, int *CodeLen_HCLEN, int dim){
	int codigosHuffman[dim];

	int x, max_bits=0, code = 0;

	// determinar o comprimento máximo
	for(x=0; x<dim; x++){
            //printf("\ntrololololo %d", CodeLen_HCLEN[x]);
        if((CodeLen_HCLEN[x]) > max_bits){
			max_bits = CodeLen_HCLEN[x];
		}
	}

    char **arrayHuffman;
    arrayHuffman = (char **)malloc(sizeof(char *)*dim);
    int l;
    for (l=0;l<dim;l++){
        arrayHuffman[l] = (char *)malloc(sizeof(char)*100);
    }

	int ocorrencias[max_bits+1];
	int next_code[max_bits+1];




	//inicializar a 0
	for(x=0; x<max_bits+1; x++)
		ocorrencias[x] = 0;

	//contagem de ocorrências do comprimento de códigos no alfabeto
	for(x=0; x<dim; x++){
		ocorrencias[CodeLen_HCLEN[x]]++;
	}

	// quando o comprimento é 0 não se conta devido à codificação do next_code
	ocorrencias[0]=0;

	/*
	 * Código na documentação *alterado*
	 * Determina a "base" da codificação de Huffman
	 * 			Comprimento 		 Base
	 * 				3				  000
	 * 				4				 1100
	 * 				5				11100
	 */
	for(int bits = 1; bits <= max_bits; bits++){
		code = (code + ocorrencias[bits-1]) << 1;
		next_code[bits] = code;
	}
	for(x=0; x<dim; x++){
		// Só insere se o comprimento for diferente de 0
		if(ocorrencias[CodeLen_HCLEN[x]] != 0){
			/*
			 * Utilizando a Base determinada anteriormente, vai codificar de acordo com os comprimentos
			 * de código lidos do ficheiro
			 */
			codigosHuffman[x] = next_code[CodeLen_HCLEN[x]]++;
			addNode(Huffman_tree,int2Binary(codigosHuffman[x],CodeLen_HCLEN[x]),x,1);

			strcpy(arrayHuffman[x], int2Binary(codigosHuffman[x],CodeLen_HCLEN[x]));
		}
	}
	return arrayHuffman;
}

/*2ª semana - PONTO 4 e PONTO 5*/
//devolve o índice dada a árvore
int indexFromTree(HuffmanTree *Huffman_tree){
    int isNotLeaf = -2;
    int indice = isNotLeaf;
    char bit;
    while (indice == isNotLeaf){
        bit = readBlockFormat(need_1);
        indice = nextNode(Huffman_tree,bit);
    }
    resetCurNode(Huffman_tree);
    return indice;
}

/*
*Devolve um array contendo os comprimentos de código
*referentes ao alfabeto de literais/comprimentos, dada a
*dimensão do array e a árvore que armazena os comprimentos
*de código
*/
int* LenCode (int dim, HuffmanTree *Huffman_tree){
    int *lengths = new int[dim];
    printf("\nlolitos %d", lengths[0]);
    for(int i = 0;i<dim;){
        int indice = indexFromTree(Huffman_tree);
        int repeat = 0;
        if (indice <= 15)
            lengths[i++] = indice;
        else{
            switch(indice){
            case 16:
                repeat = readBlockFormat(extra_16) + 3;
                for (int x = 0; x<repeat;x++){
                    lengths[i] = lengths[i-1];
                    i++;
                }
                break;
            case 17:
                repeat = readBlockFormat(extra_17) + 3;
                for (int x = 0; x<repeat;x++){
                    lengths[i++] = 0;
                }
                break;
            case 18:
                repeat = readBlockFormat(extra_18) + 11;
                for (int x = 0; x < repeat;x++){
                    lengths[i++] = 0;
                }
                break;
            }
        }
    }
    printf("\nlolitos2 %d", lengths[0]);
    return lengths;
}


//---------------------------------------------------------------
//Lê o cabeçalho do ficheiro gzip: devolve erro (-1) se o formato for inválidodevolve, ou 0 se ok
int getHeader(gzipHeader *gzh) //obtém cabeçalho
{
	unsigned char byte;

	//Identicação 1 e 2: valores fixos
	fread(&byte, 1, 1, gzFile);
	(*gzh).ID1 = byte;
	if ((*gzh).ID1 != 0x1f) return -1; //erro no cabeçalho

	fread(&byte, 1, 1, gzFile);
	(*gzh).ID2 = byte;
	if ((*gzh).ID2 != 0x8b) return -1; //erro no cabeçalho

	//Método de compressão (deve ser 8 para denotar o deflate)
	fread(&byte, 1, 1, gzFile);
	(*gzh).CM = byte;
	if ((*gzh).CM != 0x08) return -1; //erro no cabeçalho

	//Flags
	fread(&byte, 1, 1, gzFile);
	unsigned char FLG = byte;

	//MTIME
	char lenMTIME = 4;
	fread(&byte, 1, 1, gzFile);
	(*gzh).MTIME = byte;
	for (int i = 1; i <= lenMTIME - 1; i++)
	{
		fread(&byte, 1, 1, gzFile);
		(*gzh).MTIME = (byte << 8) + (*gzh).MTIME;
	}

	//XFL (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).XFL = byte;

	//OS (not processed...)
	fread(&byte, 1, 1, gzFile);
	(*gzh).OS = byte;

	//--- Check Flags
	(*gzh).FLG_FTEXT = (char)(FLG & 0x01);
	(*gzh).FLG_FHCRC = (char)((FLG & 0x02) >> 1);
	(*gzh).FLG_FEXTRA = (char)((FLG & 0x04) >> 2);
	(*gzh).FLG_FNAME = (char)((FLG & 0x08) >> 3);
	(*gzh).FLG_FCOMMENT = (char)((FLG & 0x10) >> 4);

	//FLG_EXTRA
	if ((*gzh).FLG_FEXTRA == 1)
	{
		//ler 2 bytes XLEN + XLEN bytes de extra field
		//1º byte: LSB, 2º: MSB
		char lenXLEN = 2;

		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).xlen = (byte << 8) + (*gzh).xlen;

		(*gzh).extraField = new unsigned char[(*gzh).xlen];

		//ler extra field (deixado como está, i.e., não processado...)
		for (int i = 0; i <= (*gzh).xlen - 1; i++)
		{
			fread(&byte, 1, 1, gzFile);
			(*gzh).extraField[i] = byte;
		}
	}
	else
	{
		(*gzh).xlen = 0;
		(*gzh).extraField = 0;
	}

	//FLG_FNAME: ler nome original
	if ((*gzh).FLG_FNAME == 1)
	{
		(*gzh).fName = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no máximo 1024 caracteres no array
				(*gzh).fName[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fName[1023] = 0;  //apesar de nome incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fName = 0;

	//FLG_FCOMMENT: ler comentário
	if ((*gzh).FLG_FCOMMENT == 1)
	{
		(*gzh).fComment = new char[1024];
		unsigned int i = 0;
		do
		{
			fread(&byte, 1, 1, gzFile);
			if (i <= 1023)  //guarda no máximo 1024 caracteres no array
				(*gzh).fComment[i] = byte;
			i++;
		}while(byte != 0);
		if (i > 1023)
			(*gzh).fComment[1023] = 0;  //apesar de comentário incompleto, garantir que o array termina em 0
	}
	else
		(*gzh).fComment = 0;


	//FLG_FHCRC (not processed...)
	if ((*gzh).FLG_FHCRC == 1)
	{
		(*gzh).HCRC = new unsigned char[2];
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[0] = byte;
		fread(&byte, 1, 1, gzFile);
		(*gzh).HCRC[1] = byte;
	}
	else
		(*gzh).HCRC = 0;

	return 0;
}


//---------------------------------------------------------------
//Analisa block header e vê se é huffman dinâmico
int isDynamicHuffman(unsigned char rb)
{
	unsigned char BTYPE = rb & 0x03;

	if (BTYPE == 0) //--> sem compressão
	{
		printf("Ignorando bloco: sem compactação!!!\n");
		return 0;
	}
	else if (BTYPE == 1)
	{
		printf("Ignorando bloco: compactado com Huffman fixo!!!\n");
		return 0;
	}
	else if (BTYPE == 3)
	{
		printf("Ignorando bloco: BTYPE = reservado!!!\n");
		return 0;
	}
	else
		return 1;
}


//---------------------------------------------------------------
//Obtém tamanho do ficheiro original
long getOrigFileSize()
{
	//salvaguarda posição actual do ficheiro
	long fp = ftell(gzFile);

	//últimos 4 bytes = ISIZE;
	fseek(gzFile, -4, SEEK_END);

	//determina ISIZE (só correcto se cabe em 32 bits)
	unsigned long sz = 0;
	unsigned char byte;
	fread(&byte, 1, 1, gzFile);
	sz = byte;
	for (int i = 0; i <= 2; i++)
	{
		fread(&byte, 1, 1, gzFile);
		sz = (byte << 8*(i+1)) + sz;
	}

	//restaura file pointer
	fseek(gzFile, fp, SEEK_SET);

	return sz;
}


//---------------------------------------------------------------
void bits2String(char *strBits, unsigned char byte)
{
	char mask = 0x01;  //get LSbit

	strBits[8] = 0;
	for (char bit, i = 7; i >= 0; i--)
	{
		bit = byte & mask;
		strBits[i] = bit +48; //converter valor numérico para o caracter alfanumérico correspondente
		byte = byte >> 1;
	}
}

/*
*Função que converte um número inteiro para binário
*e devolve num array de char
*/
char *int2Binary(int number, int LenCode){
    string out = "";
    int countbit = 0;
    while (number != 0){
        ostringstream convert;
        convert << number%2;
        out =  convert.str() + out;
        number = number/2;
        countbit++;
    }
    for (int i = countbit; i<LenCode; i++ ){
        out = "0" + out;
    }
    char *res = new char[LenCode+1];
    strncpy(res, out.c_str(), LenCode);
    res[LenCode] = 0;
    return res;
}
