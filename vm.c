//Virtual Machine:
//Used to process custom commands
#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

int main(void){

	//Create area for stack and code text area
	//Must be inside main or else won't run
	bp = sp = malloc(sizeof(int)*100);
	pc = cmd= malloc(sizeof(int)*100);

	/* 
	   bp ------------
	   ... 
	   !______ 
	   !___PC
	   sp/bp -> !___BP
	   !______-1
	   sp       !______
	   */

	// pc --------------
	*cmd = ENT; cmd++;
	*cmd = 1  ; cmd++;
	*cmd = LEA; cmd++;

	*cmd = -1 ; cmd++;
	*cmd = PSH; cmd++;
	*cmd = IMM; cmd++;
	*cmd = 8  ; cmd++; //eax = 8
	*cmd = SI ; cmd++; 

	*cmd = IMM; cmd++;
	*cmd = 8  ; cmd++;
	*cmd = PSH; cmd++;

	*cmd = LEA; cmd++;
	*cmd = -1 ; cmd++;
	*cmd = LI ; cmd++; 

	*cmd = ADD; cmd++;

	*cmd = EXIT; cmd++;

	while(1){ //Infinitely Run commands till finish
		printf("before exec:pc %llx *pc %llx bp %llx eax:%d *sp: %llx sp %llx \n", pc, *pc, bp, eax, sp, *sp);	
		inst = *pc++;
		//define:
		//IF TAKE PARAM: pc++;
		// CMD   3  ...
		//  |    |   |
		// inst  pc  pc+1
		if(inst == ENT){*--sp = (int)bp; bp = sp; sp = sp - *pc; pc++;} 
		if(inst == LEV){sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}
		if(inst == LEA){eax = (int)(bp + *pc++);}
		if(inst == IMM){eax = *pc; pc++;}
		if(inst == PSH){*--sp = eax;}
		if(inst == LI ){eax = *(int *)eax;}
		if(inst == SI ){*(int *)*sp++ = eax;}

		if(inst == JMP){pc = (int *)*pc;} //pc and not pc++ since NO NEED TO MOVE
		if(inst == JSR){eax = *(int *)eax;}
		if(inst == BZ ){eax = *(int *)eax;}
		if(inst == BNZ ){eax = *(int *)eax;}

		//expr
		if(inst == ADD){eax = *sp++ + eax;}
		if(inst == SUB){eax = eax - *sp++;}
		if(inst == MUL){eax = eax * *sp++;}
		if(inst == DIV){eax = eax / *sp++;}

		printf("%.4s",
				&"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
				"OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
				"OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[inst * 5]);
		printf("\n");
		for(int i=0; i<10; i++) printf(" %llx ", *(bp-i));
		printf("\n");

		if(inst == EXIT){return 0;}
	}
}
