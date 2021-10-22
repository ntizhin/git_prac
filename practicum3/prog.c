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
	else if(!strcmp(T -> key, wrd)){	
		T -> count += 1;
		free(wrd);
	}
	else if(strcmp(T -> key, wrd) > 0) T -> left = addTree(T -> left, wrd);
	else if(strcmp(T -> key, wrd) < 0) T -> right = addTree(T -> right, wrd);
	return T;
}

int maxTree(struct Tree *T){
	if(T != NULL){
		int maxL = maxTree(T -> left), maxR = maxTree(T -> right), maxT = T -> count;
		if(maxL > maxT) maxT = maxL;
		if(maxR > maxT) maxT = maxR;  
		return maxT;
	}
	return 0;
}

void maxTwrite(struct Tree *T, FILE *f, int n, int maxT){
	if(T != NULL){
		if(T -> count == maxT){
			fprintf(f, "%s %d %.7f\n", T -> key, T -> count, (float)(T -> count)/n );
			T -> count = 0;
		}
		maxTwrite(T -> left, f, n, maxT);
		maxTwrite(T -> right, f, n, maxT);
	}
}

void TreeWriteF(struct Tree *T, FILE *f, int n){
	int maxT;
	for(; maxT = maxTree(T); maxTwrite(T, f, n, maxT));
}

int sum(struct Tree *T){
	if(T == NULL) return 0;
	return T -> count + sum(T -> left) + sum(T -> right);
}

void delTree(struct Tree *T){
	if(T != NULL){
		delTree(T -> left);
		T -> left = NULL;
		delTree(T -> right);
		T -> right = NULL;
		free(T -> key);
		free(T);
	}
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
				wrd = realloc(wrd, (len + 1) * sizeof(char));
				T = addTree(T, wrd);
			}
			else free(wrd);
			if(ispunct(c)){
			char *wrd = malloc(2 * sizeof(char));
			wrd[0] = c;
			wrd[1] = 0;
			T = addTree(T, wrd);
		}
	}
	int n;
	n = sum(T);
	TreeWriteF(T, f2, n);
	
	delTree(T);
	T = NULL;
	
	fclose(f1);
	fclose(f2);
	return 0;
}
