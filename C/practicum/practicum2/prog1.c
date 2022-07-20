#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int codingUTF8(unsigned int U32, unsigned char (*U8)[4]){
	if(U32 <= 0x7F){
		(*U8)[0] = U32;
		return 1;
	}
	else if(U32 <= 0x7FF){
		char byte1 = 0xC0;
		char byteN = 0x80;
		(*U8)[0] = byte1 + ((U32 >> 6) & 0x1F);
		(*U8)[1] = byteN + (U32 & 0x3F);
		return 2;
	}
	else if(U32 <= 0xFFFF){
		char byte1 = 0xE0;
		char byteN = 0x80;
		(*U8)[0] = byte1 + ((U32 >> 12) & 0x0F);
		(*U8)[1] = byteN + ((U32 >> 6) & 0x3F);
		(*U8)[2] = byteN + (U32 & 0x3F);
		return 3;
	}
	else{
		char byte1 = 0xF0;
		char byteN = 0x80;
		(*U8)[0] = byte1 + ((U32 >> 18) & 0x07);
		(*U8)[1] = byteN + ((U32 >> 12) & 0x3F);
		(*U8)[2] = byteN + ((U32 >> 6) & 0x3F);
		(*U8)[3] = byteN + (U32 & 0x3F);
		return 4;
	}
}

unsigned short int swapBytes(unsigned short int p){
	return (p << 8) + (p >> 8);
}


int main(int agc, char *argV[]){
	enum marker{LE, BE} mrk;
	int n, i;
	unsigned short U16, next;
	unsigned int U32;
	unsigned char b[4];
	unsigned char m[2];
	
// INSTALL FILE //
	FILE *f1, *f2;	
	f1 = stdin;
	f2 = stdout;
	for(i = 0; i < agc; i++){
		if(!strcmp(argV[i], "-i")) f1 = fopen(argV[i + 1], "r");
		if(!strcmp(argV[i], "-o")) f2 = fopen(argV[i + 1], "w");
	};
	if((f1 == NULL) | (f2 == NULL)){
		printf("Cannot open file.\n");
		return 0;
	}	
	
	
// INSTALL MARKER //	
	mrk = LE;
	n = fread(&m, sizeof(unsigned char), 2, f1);
	if(n == 2){
		if((m[0] == 0xFE) & (m[1] == 0xFF)) mrk = BE;
		else if(!((m[0] == 0xFF) && (m[1] == 0xFE))){
			for(i = 0; i < agc; i++){
				if(!strcmp(argV[i], "-le")) mrk = LE;
				if(!strcmp(argV[i], "-be")) mrk = BE;
			}
			fseek(f1, SEEK_SET, 0);
		}
	}
	else fseek(f1, SEEK_SET, 0);

// Start //	
	while(!feof(f1)){
		n = fread(&U16, sizeof(unsigned short int), 1, f1);
		if(mrk == BE) U16 = swapBytes(U16);
		if(n == 0){
			if(!feof(f1)){
				fprintf(stderr, "error, first word: %ld\n", ftell(f1) - 2); 
				fseek(f1, SEEK_CUR, -1);
				continue;	
			}
			else break;
		}
		else{
			 if((U16 < 0xD800) || (U16 > 0xDFFF)) U32 = U16;
			 else if(U16 >= 0xDC00){
				fprintf(stderr, "error, first word: %ld\n", ftell(f1) - 2);
				fseek(f1, SEEK_CUR, -1);
				continue;
			}
			 else{
				U32 = (U16 - 0xD800);
				U32 = U32 << 10;
				n = fread(&next, sizeof(unsigned short int), 1, f1);
				if(mrk == BE) next = swapBytes(next);
				if((n == 0) || (next < 0xDC00) | (next > 0xDFFF)){
					fprintf(stderr, "error, second word: %ld\n", ftell(f1) - 4);
					fseek(f1, SEEK_CUR, -3);
					continue;
				}
				else U32 = (U32 + (next - 0xDC00)) + 0x10000;
			 }	
		}
		i = codingUTF8(U32, &b);
		fwrite(&b, sizeof(unsigned char), i, f2);
	}
	
	if(ftell(f1) % 2) fprintf(stderr, "error: odd amount\n"); 
	
	fclose(f1);
	fclose(f2);
	return 0;
}
