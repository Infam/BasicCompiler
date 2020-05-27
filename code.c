#include <stdio.h>
int f1(int g, int f){
	int l;
	l = g + f;
}

int f2(){ //gbl
	int a; //gbl
	a = 4; //stmt ->expr
	f1(a, 3); //expr
}
