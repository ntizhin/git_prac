#include <stdio.h>
#include <stdlib.h>

char* EntrS(){
	enum {ExpansionPortion = 200};
	int count = 1, c, memory = ExpansionPortion;
	char *p = malloc(memory * sizeof(char));
	char *q = p;
	while((c = getchar()) != EOF){
		if(count > memory){
			memory += ExpansionPortion;
			p = realloc(p, memory * sizeof(char));
			q = p + count - 1;
		}
		*q = (char)c;
		count++;
		q++;
		if(c == '\n') break;
	}
	*q = 0;
	p = realloc(p, count * sizeof(char));
	return p;
}

int mystrcmp(char *s1, char *s2){
	for(; (*s1 == *s2) && (*s1 != 0) && (*s2 != 0); s1++, s2++);
	if(*s1 < *s2) return -1;
	if(*s1 > *s2) return 1;
	return 0;
}

void qs(char *pp[], int first, int last)
{
    if (first < last)
    {
        int left = first, right = last;
        char *middle = pp[(left + right) / 2];
        do
        {
            while (mystrcmp(pp[left], middle) < 0) left++;
            while (mystrcmp(pp[right], middle) > 0) right--;
            if (left <= right)
            {
                char *tmp = pp[left];
                pp[left] = pp[right];
                pp[right] = tmp;
                left++;
                right--;
            }
        } while (left <= right);
        qs(pp, first, right);
        qs(pp, left, last);
    }
}

void printArrS(char *pp[], int size){
	char *q;
	int i;
	for(i = 0; i < size; i++){
		q = pp[i];
		for(; *q; q++)
			putchar(*q);
	}
}

int main(){	
	int c, size;
	char **pp = malloc(sizeof(char*));
	for(size = 0; (c = getchar()) != EOF; size++){
		ungetc(c, stdin);
		pp = realloc(pp, (size + 1)*sizeof(char*));
		*(pp + size) = EntrS();	
	}
	qs(pp, 0, size - 1);
	printArrS(pp, size);
	for(; size; size--) free(pp[size - 1]);
	free(pp);
	return 0;

}


