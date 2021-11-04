#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int c_in_arr(int c, char arr[]){
    int n;
    for(n = sizeof(arr) - 1; n > 0; n--, arr++){
        if((char)c == *arr) return 1;
    }
    return 0;
}

void print_arr(char **arr, int n){
    for(; n > 0; n--, arr++){
		printf("%s\n", *arr);
		free(*arr);
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
	#define prt 10
	int c, len, n = 0;
	char **arr_word = malloc(1);
    char spec[] = {'&', '|', ';', '>', '<', '(', ')'}; // &&, ||, <<, >> обрабатываются отдельно
	char *word;
	
	while(!feof(f1)){
		for(; (c = fgetc(f1)) == ' ';);
		len = 0;
		word = malloc(prt * sizeof(char));
		while( (c != EOF) && (c != ' ') && (c != '\n') && !c_in_arr(c, spec) ){
			if(c != '"'){
				if(! ((len + 1) % prt) ) word = realloc(word, (len + 1 + prt) * sizeof(char));
				word[len] = c;
				len += 1;
			}
			else{
				while( ((c = fgetc(f1)) != '"') && (c != EOF) ){
					if(! ((len + 1) % prt) ) word = realloc(word, (len + 1 + prt) * sizeof(char));
					word[len] = c;
					len += 1;
				}
			}
			c = fgetc(f1);
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
            int next;
            if(((c == '&') || (c == '|') || (c == '<') || (c == '>'))){
                if(c == (next = fgetc(f1))){
					word = realloc(word, 3 * sizeof(char));
					word[1] = next;
					word[2] = 0;
				}
				else ungetc(next, f1);
			}
            n += 1;
            arr_word = realloc(arr_word, n * sizeof(char*));
            arr_word[n - 1] = word; 
		}
		if((c == '\n') || (c == EOF)){
			print_arr(arr_word, n);
			n = 0;
		}
	}
    free(arr_word);
	fclose(f1);
	fclose(f2);
	return 0;
}