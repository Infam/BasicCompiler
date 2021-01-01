//Virtual Machine:
//Used to process custom commands
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "compiler.h"

struct table tab[DATASZ] = {
	{ Char,     "char", 0,0,0},
	{ Else,     "else", 0,0,0},
	{ Enum,     "enum", 0,0,0},
	{ If,         "if", 0,0,0},
	{ Int,       "int", 0,0,0},
	{ Return, "return", 0,0,0},
	{ Sizeof, "sizeof", 0,0,0},
	{ While,   "while", 0,0,0},
	{ 0, "printf", Sys,Int,PRTF},
	{0} //set all elements to zero, easy to check if filled and use while loops
};

// File
int fd;

// Debug
char *lp; //Line Pointer
int ln = 0; //Line Pointer
int *cnt; //count of command: used to print commands

// Pointers
int *sp, *bp, *pc, *cmd, *sysc; //stack pointers, and machine code pointers
int eax, inst; // eax register, inst = (pc -1)

//Actual text
char *str; 
char *sstr; //Start of string
char *tp; //text pointer

//Symbol table & Next call:
//tk will hold both characters and integer values
int tk; //the token type
int addr = 0; //will hold index of wanted item
int mainaddr = -1; //Holds address of main function
int type; // token type: int abc
int val; //value of found integers, i.e. int a = 20
int parmc; //function parameter count
int varc; //Local Variable Counter

char * gdata; //Global data area & strings

int main(void){


	//Create area for stack and code text area
	//Must be inside main or else won't run
	str  = malloc(sizeof(char)*DATASZ);
	gdata= malloc(sizeof(char)*DATASZ);
	bp = sp = malloc(sizeof(int)*DATASZ);
	cmd= malloc(sizeof(int)*DATASZ);

	if ((fd = open("code.c", 0)) < 0) { printf("cannot open code file.\n"); return -1; }
	if ((read(fd, str, DATASZ)) <= 0) { printf("cannot read code file.\n"); return -1; }

	tp = str;
	lp = tp;
	cnt = cmd;

	next();
	while(tk) //While have token
		glbl();
	print();

	check(mainaddr < 0,"No Main Function.");

	pc = (int *)tab[mainaddr].Value;

	while(1){ //Infinitely Run commands till finish
		inst = *pc++;
		//define:
		//IF TAKE PARAM: pc++;
		// CMD   3  ...
		//  |    |   |
		// inst  pc  pc+1
		if(inst == ENT){*--sp = (int)bp; bp = sp; sp = sp - *pc; pc++;} 
		else if(inst == LEV){sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}
		else if(inst == LEA){eax = (int)(bp + *pc++);} //gets address using address +- offset
		else if(inst == IMM){eax = *pc; pc++;}
		else if(inst == PSH){*--sp = eax;} //push value to stack and move
		else if(inst == LI ){eax = *(int *)eax;} // Cast eax to pointer type in order to get its contents
		else if(inst == SI ){*(int *)*sp++ = eax;}

		else if(inst == JMP){pc = (int *)*pc;} //pc and not pc++ since NO NEED TO MOVE
		else if(inst == JSR){*--sp = (int)(pc + 1); pc = (int *)*pc;} //Changing frames, PSH pc and JMP
		else if(inst == BZ ){if(eax){pc = pc + 1;}else{pc = (int *)*pc;}} // Jump if false, walk if true
		else if(inst == BNZ ){if(!eax){pc = pc + 1;}else{pc = (int *)*pc;}} // Jump if false, walk if true
		else if(inst == ADJ){sp = sp + *pc; pc++;} //Move sp back over parameters

		else if(inst == PRTF){sysc = sp + pc[1]; printf((char*)sysc[-1], sysc[-2], sysc[-3], sysc[-4]); printf("\n");} // Print out eax pc[1] = x: PRFT -> ADJ x

		//expr
		else if(inst == ADD){eax = *sp++ + eax;}
		else if(inst == SUB){eax = eax - *sp++;}
		else if(inst == MUL){eax = eax * *sp++;}
		else if(inst == DIV){eax = eax / *sp++;}
		else if(inst == EQ) {eax = *sp++ == eax;}
		else if(inst == NE) {eax = *sp++ != eax;}
		else if(inst == GE) {eax = *sp++ >= eax;}
		else if(inst == LE) {eax = *sp++ <= eax;}
		else if(inst == GT) {eax = *sp++ > eax;}
		else if(inst == LT) {eax = *sp++ < eax;}

		if(inst == EXIT){return 0;}

	}
}
