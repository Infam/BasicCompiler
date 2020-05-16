//Virtual Machine:
//Used to process custom commands
#include <stdio.h>
#include <stdlib.h>
#define int long long //So converting from int pointer to int has no issues

//Q:
//Why cmd and pc?

//Commands of Machine
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

// Pointers
int *sp, *bp, *pc, *cmd;

int tk, eax; // eax register
	
//test



int main(){
//Create area for stack and code text area
//Must be inside main or else won't run
	bp = sp = malloc(sizeof(int)*100);
	pc = cmd= malloc(sizeof(int)*100);
	
	*cmd = LEA; cmd++;
	*cmd = 1  ; cmd++;
	*cmd = IMM; cmd++;
	*cmd = 10 ; cmd++;

	while(1){ //Infinitely Run commands till finish
		//define:
		if(tk == IMM){}
		if(tk == IMM){}
		if(tk == IMM){}

	}
	return 0;
}
