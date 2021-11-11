#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int c_in_arr(char c, char arr[]){
    int n;
    for(n = sizeof(arr) - 1; n > 0; n--, arr++){
        if((char)c == *arr) return 1;
    }
    return 0;
}

char **PrStr(char *str, int *count){
	int prt = 10;
	int c, len, n = 0;
	char **arr_word = malloc(0);
    char spec[] = {'&', '|', ';', '>', '<', '(', ')'}; // &&, ||, >> обрабатываются отдельно
	char *word;

	while(*str){
		for(; *str == ' '; str++);
		len = 0;
		word = malloc(prt * sizeof(char));
		while( (c = *str) && (c != ' ') && !c_in_arr(c, spec) ){
			if(c != '"'){
				if(! ((len + 1) % prt) ) word = realloc(word, (len + 1 + prt) * sizeof(char));
				word[len] = c;
				len += 1;
			}
			else{
				while( ((c = *++str) != '"') && c ){
					if(! ((len + 1) % prt) ) word = realloc(word, (len + 1 + prt) * sizeof(char));
					word[len] = c;
					len += 1;
				}
				if(!c) break;
			}
			
			str++;
		}
		if(len > 0){
			word[len] = 0;
            word = realloc(word, (len + 1) * sizeof(char));
            n += 1;
            arr_word = realloc(arr_word, n * sizeof(char*));
            arr_word[n - 1] = word; 
		}
        else free(word);
		if(c_in_arr(c, spec)){
    	    word = malloc(2 * sizeof(char));
            word[0] = c;
			word[1] = 0;
            if(((c == '&') || (c == '|') || (c == '>'))){
				str++;
				if(c == *str){
					word = realloc(word, 3 * sizeof(char));
					word[1] = c;
					word[2] = 0;
				}
				else str--;
			} 
            n += 1;
            arr_word = realloc(arr_word, n * sizeof(char*));
            arr_word[n - 1] = word; 
			str++;
		}
	}
	*count = n;
	return arr_word;
}

char* EntrStr(FILE *f){
	#define ExpansionPortion  20;
	int count = 1, c, memory = ExpansionPortion;
	char *p = malloc(memory * sizeof(char));
	char *q = p;
	while(((c = fgetc(f)) != EOF) && (c != '\n')){
		if(count > memory){
			memory += ExpansionPortion;
			p = realloc(p, memory * sizeof(char));
			q = p + count - 1;
		}
		*q = (char)c;
		count++;
		q++;
	}
	*q = 0;
	p = realloc(p, count * sizeof(char));
	return p;
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
	char *str;
	char **arr_word;
	int n;
	while(!feof(f1)){
		str = EntrStr(f1);
		arr_word = PrStr(str, &n);
		free(str);
		for(i = 0; i < n; i++){
			printf("%s\n", arr_word[i]);
			free(arr_word[i]);
		}
		free(arr_word);
		arr_word = NULL;
	}
	fclose(f1);
	fclose(f2); 
	return 0;
}