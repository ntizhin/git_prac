#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Tree{
	char *key;
	int count;
	struct Tree *left, *right;
};


struct Tree *addTree(struct Tree *T, char *wrd){
	if(T == NULL){
		T = calloc(1, sizeof(struct Tree));
		T -> key = wrd;
		T -> count = 1;
		T -> right = NULL;
		T -> left = NULL;
	}
	else if(!strcmp(T -> key, wrd)) T -> count += 1;
	else if(strcmp(T -> key, wrd) > 0) T -> left = addTree(T -> left, wrd);
	else if(strcmp(T -> key, wrd) < 0) T -> right = addTree(T -> right, wrd);
	return T;
}

void TreeWriteF(struct Tree *T, FILE *f, int n){
	if(T != NULL){
		TreeWriteF(T -> left, f, n);
		fprintf(f, "%s %d %.7f\n", T -> key, T -> count, (float)(T -> count)/n );
		TreeWriteF(T -> right, f, n);
	}
}

int sum(struct Tree *T){
	if(T == NULL) return 0;
	return T -> count + sum(T -> left) + sum(T -> right);
}

int main(int agc, char *argV[]){
	int i;
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
	
	// General part //
	
	int c;
	struct Tree *T = NULL;
	const int prt = 20;
	int len = 0;
	char *wrd;
	while(!feof(f1)){
		for(; isspace(c = fgetc(f1)) && (c != EOF););
			len = 0;
			wrd = malloc(prt * sizeof(char));
			while(!( (c == EOF) || isspace(c) || ispunct(c) )){
				if(! ((len + 1) % prt) ) wrd = realloc(wrd, (len + 1 + prt) * sizeof(char));
				wrd[len] = c;
				len += 1;
				c = fgetc(f1);
			}
			if(len > 0){
				wrd[len] = 0;
				T = addTree(T, wrd);
			}
			if(ispunct(c)){
			char *wrd = malloc(2 * sizeof(char));
			wrd[0] = c;
			wrd[1] = 0;
			T = addTree(T, wrd);
		}
	}
	int n;
	n = sum(T);
	printf("\n %d \n", n);
	TreeWriteF(T, f2, n);
	return 0;
}
