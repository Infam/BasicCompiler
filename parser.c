//Parser:
//Used to turn code into tokens & beginning the process of converting to machine code.
#include <stdio.h>
#include <stdlib.h>
#define int long long //So converting from int pointer to int has no issues

//value and token
//tk will hold both characters and integer values
int val, tk;

char *str = "// hello! ";

//Starting enum from 128:
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
}; 

//Get next useful token
void next(){
	while(1){ // while no useful token found
		tk = *str++;
		//Newline
		if(tk == '\n'){
			tk = *str++;
		}
		//Comment or Division
		//Faster to check for cases then to let next run through all other possibilities
		if(tk == '/'){
			if(*str == '/'){
				while(*str != '\n' && *str != '0'){
					str++;
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
			while(*str != '\n' && *str != '0'){
				str++;
			}
		}
		
	}



}
