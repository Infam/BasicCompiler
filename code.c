#include <stdio.h>
int main(){
	int *a;
	int b;	
	b = 5;
	a = &b;
	while(*a < 8){
		printf("%d",*a);
		*a = *a + 1;
	}
}
