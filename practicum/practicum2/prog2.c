#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned short int swapBytes(unsigned short int p){
	return (p << 8) + (p >> 8);
}

enum marker{LE, BE};
int codingUTF16(unsigned int U32, unsigned int *U16, enum marker mrk){
	if(mrk == LE){
		if(U32 < 0x10000){
			*U16 = U32;
			return 1;
		}
		else{
			unsigned short int prt1, prt2; //prt1 - older word, prt2 - younger word;
			unsigned int res;
			U32 -= 0x10000;
			prt1 = (U32 >> 10) + 0xD800;
			prt2 = (U32 & 0x3ff) + 0xDC00;
			res = prt1;
			res = (res << 8 * sizeof(short int)) + prt2;
			*U16 = res;
			return 2;
		}
	}
	else{
		if(U32 < 0x10000){
			unsigned short int tmp = U32;
			*U16 = swapBytes(tmp);
			return 1;
		}
		else{
			unsigned short int prt1, prt2; //prt1 - older word, prt2 - younger word;
			unsigned int res;
			U32 -= 0x10000;
			prt1 = swapBytes((U32 >> 10) + 0xD800);
			prt2 = swapBytes((U32 & 0x3ff) + 0xDC00);
			res = prt1;
			res = (res << 8 * 2) + prt2;
			*U16 = res;
			return 2;
		}
	}
}



int main(int agc, char *argV[]){
	enum marker mrk;
	int n, i;
	unsigned int U32;
	unsigned int U16;
	unsigned char U8;
	unsigned char m[3];
	
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
	for(i = 0; i < agc; i++) if(!strcmp(argV[i], "-be")) mrk = BE;
	switch(mrk)
    {
    case LE:
        fprintf(f2, "%c%c", 0xFF, 0xFE);
        break;
    case BE:
        fprintf(f2, "%c%c", 0xFE, 0xFF);
        break;
	} 
	
	n = fread(&m, sizeof(unsigned char), 3, f1);
	if(n == 3){
		if(!( (m[0] == 0xef) && ( ((m[1] == 0xbf) &&(m[2] == 0xbe)) || ((m[1] == 0xbb) && (m[2] == 0xbf))) )){
			fseek(f1, SEEK_SET, 0);
		}
	}
	else fseek(f1, SEEK_SET, 0);

// Start //	
	while((n = fread(&U8, sizeof(unsigned char), 1, f1)) != 0){
		if(U8 <= 0x7F) U32 = U8;
		else if((U8 & 0xC0) == 0x80){
			fprintf(stderr, "wrong sequence, !start! byte: %ld\n", ftell(f1) - 1);
		}
		else if((U8 & 0xE0) == 0xC0){
			U32 = U8 & 0x1F;
			n = fread(&U8, sizeof(unsigned char), 1, f1);
			if((n == 0) | ((U8 & 0xC0) != 0x80)){
				fprintf(stderr, "wrong sequence, !next! byte: %ld\n", ftell(f1) - 1);
				fseek(f1, SEEK_CUR, -1);
			}
			else U32 = (U32 << 6) + (U8 & 0x3F);
		}
		else if((U8 & 0xF0) == 0xE0){
			U32 = U8 & 0x0F;
			n = fread(&U8, sizeof(unsigned char), 1, f1);
			if((n == 0) | ((U8 & 0xC0) != 0x80)){
				fprintf(stderr, "wrong sequence, !next! byte: %ld\n", ftell(f1) - 1);
				fseek(f1, SEEK_CUR, -1);
			}
			else{
				U32 = (U32 << 6) + (U8 & 0x3F);
				n = fread(&U8, sizeof(unsigned char), 1, f1);
				if((n == 0) | ((U8 & 0xC0) != 0x80)){
					fprintf(stderr, "wrong sequence, !next! byte: %ld\n", ftell(f1) - 1);
					fseek(f1, SEEK_CUR, -1);
				}
				else U32 = (U32 << 6) + (U8 & 0x3F);
			}
		}
		else if((U8 & 0xF8) == 0xF0){
			U32 = U8 & 0x7;
			n = fread(&U8, sizeof(unsigned char), 1, f1);
			if((n == 0) | ((U8 & 0xC0) != 0x80)){
				fprintf(stderr, "wrong sequence, !next! byte: %ld\n", ftell(f1) - 1);
				fseek(f1, SEEK_CUR, -1);
			}
			else{
				U32 = (U32 << 6) + (U8 & 0x3F);
				n = fread(&U8, sizeof(unsigned char), 1, f1);
				if((n == 0) | ((U8 & 0xC0) != 0x80)){
					fprintf(stderr, "wrong sequence, !next! byte: %ld\n", ftell(f1) - 1);
					fseek(f1, SEEK_CUR, -1);
				}
				else{
					U32 = (U32 << 6) + (U8 & 0x3F);
					n = fread(&U8, sizeof(unsigned char), 1, f1);
					if((n == 0) | ((U8 & 0xC0) != 0x80)){
						fprintf(stderr, "wrong sequence, byte: %ld\n", ftell(f1) - 1);
						fseek(f1, SEEK_CUR, -1);
					}
					else U32 = (U32 << 6) + (U8 & 0x3F);
				}
			}
		}
		if(codingUTF16(U32, &U16, mrk) == 1){
			unsigned short int tmp = U16;
			fwrite(&tmp, sizeof(unsigned short int), 1, f2);
		}
		else fwrite(&U16, sizeof(unsigned int), 1, f2);
	}
	fclose(f1);
	fclose(f2);
	return 0;
}
