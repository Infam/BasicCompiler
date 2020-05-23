//Parser:
//Used to turn code into tokens & beginning the process of converting to machine code.
//QUESTIONS
//Use Hashing to reduce search time?
//Why still linearly search through arrays then?
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define int long long //So converting from int pointer to int has no issues
 
//value and token
//tk will hold both characters and integer values
int val, tk;
char *str = "hello c";
char *sstr; //Start of string
char *tp; //text pointer

//Starting enum from 128:
enum {
	Num = 128, Fun, Sys, Glo, Loc, Id,
	Char, Else, Enum, If, Int, Return, Sizeof, While,
	Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};


struct tab{ //symbol table
	int tktype; char* name; int class, type, value;
} tab[100] = {
	{ Char,     "char", 0,0,0},
	{ Else,     "else", 0,0,0},
	{ Enum,     "enum", 0,0,0},
	{ If,         "if", 0,0,0},
	{ Int,       "int", 0,0,0},
	{ Return, "return", 0,0,0},
	{ Sizeof, "sizeof", 0,0,0},
	{ While,   "while", 0,0,0},
	{0} //set all elements to zero, easy to check if filled and use while loops
};
int addr = 0; //will hold index of wanted item

//Get next useful token
// {, (, *, ID, 

void next(){
	while(tk = *tp++){ // while no useful token found
		if(tk == ' '){
			while(*tp == ' ' && *tp != '0'){
				tp++;
			}
		}
		//Newline
		else if(tk == '\n' || tk == '\t'){
			tk = *tp++;
		}
		//Comment or Division
		//Faster to check for cases then to let next run through all other possibilities
		else if(tk == '/'){
			if(*tp == '/'){
				while(*tp != '\n' && *tp != '0'){ // where '0' is the end of string character, basically denoting end of file
		    		tp++;
		    		}
		    	}
		    	else{
		    		tk = Div;
		    		return;
		    	}
		}
		//Define or include not handled
		//Don't need to be looked at if line was comment
		else if(tk == '#'){
		    	while(*tp != '\n' && *tp != '0'){
		    		tp++;
		    	}
		}
		//Check for ID
		//Using ASCII Values:
		else if((tk >= 'a' && tk <= 'z')||(tk >= 'A' && tk <= 'Z')||(tk == '_')){
			sstr = tp - 1; // remember start of string
					//CAN'T do sstr = tk because tk is an integer not a pointer
			while((*tp >= 'a' && *tp <= 'z')||(*tp >= 'A' && *tp <= 'Z')||(*tp == '_')||(*tp >= '0' && *tp <= '9')){
				tp++;  //find end of string	
			}
			//compare
			addr = 0;
			while (tab[addr].tktype){
				if((strlen(tab[addr].name) == tp-sstr) && !memcmp(tab[addr].name, sstr, tp-sstr)){ //if there is an entry, returns 0. !0 = 1
					tk = Id;
					return; //addr is in correct spot
				}
				addr++;
			}
			//if done with loop: new variable -> store
				tab[addr].tktype = Id;
				tab[addr].name = sstr; //only care about start, end is handled elsewhere
				tk = Id;
				return;
		}
	}
}
int main(){
	tp = str;
	do{
		next();
	}while(tk);
	return 0;
}
