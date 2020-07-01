#include <stdio.h>
int g(int a){
	return a + 3;	
}

int f(int x){
	return x*3;
}
int main(){
	int b;
	b = 0;
	b = g(b);
	b = f(b);
	printf("%d",b);
}
