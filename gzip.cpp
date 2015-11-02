/*Author: Rui Pedro Paiva
Teoria da Informação, LEI, 2007/2008*/

#include <cstdlib>

#include "gzip.h"
#define n_HLIT 1
#define n_HDIST 2
#define n_HCLEN 3

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
	origFileSize = getOrigFileSize(gzFile);


	//--- ler cabeçalho
	int erro = getHeader(gzFile, &gzh);
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
        char HLIT = readBlockFormat(n_HLIT);
        printf("HLIT = %d\n", HLIT);
        char HDIST = readBlockFormat(n_HDIST);
        printf("HDIST = %d\n", HDIST);
        char HCLEN = readBlockFormat(n_HCLEN);
        printf("HCLEN = %d\n", HCLEN);

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
        readBits = 0xF;
        break;
    }
    if (availBits < needBits)
    {
        fread(&byte, 1, 1, gzFile);
        rb = (byte << availBits) | rb;
        availBits += 8;
    }

    format = rb & (1 << needBits) - 1;

    rb = rb >> needBits;

    availBits -= needBits;
    return format;
}
//---------------------------------------------------------------
//Lê o cabeçalho do ficheiro gzip: devolve erro (-1) se o formato for inválidodevolve, ou 0 se ok
int getHeader(FILE *gzFile, gzipHeader *gzh) //obtém cabeçalho
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
long getOrigFileSize(FILE * gzFile)
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
