//Parser:
//Used to turn code into tokens & beginning the process of converting to machine code.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"
char *str = "int main(int a, int b, int c){\
	     int b, c; \
	     if(b){}\
	     else{a}\
	     ;\
	     return\
	     }"; 
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
			while (tab[addr].Tktype){
				if((strlen(tab[addr].Name) == tp-sstr) && !memcmp(tab[addr].Name, sstr, tp-sstr)){ //if there is an entry, returns 0. !0 = 1
					tk = tab[addr].Tktype;
					return; //addr is in correct spot
				}
				addr++;
			}
			//if done with loop: new variable -> store
			tab[addr].Tktype = Id;
			tab[addr].Name = sstr; //only care about start, end is handled elsewhere
			tk = Id;
			return;
		}
		else if(tk >= '0' && tk <= '9'){
			val = tk;
			while(*tp >= '0' && tk <= '9')
				val = val*10 + *tp++;
			tk = Num;
			return;
		}
		else if(tk == '}' || tk == '{' || tk == ')' || tk == '(' || tk == ',' || tk == '\"' || tk == '\'' || tk == ';') return;
	}
}

void check(int expr, char *errmsg){
	if (expr){
		printf("%s\n", errmsg);
		exit(-1);
	}
}

void stmt(){ //tk is at start
	//TODO: Check where expr ends after writing it: expr + next needed?
	int *block1, *block2;
	//Have stmt take care of { and } for easy recursion
	check(tk != If && tk != Else && tk != While && tk != Return && tk != ';' && tk != '{' && tk != '}' && tk != Id, "Bad Statement: Expecting keyword/;"); 
	if(tk == ';'){
		next();
		return;
	}
	if(tk == '{'){ //if(){} //Should ALWAYS be recursively called
		next();
		while(tk != '}' && tk != 0)
			stmt();
		check(tk == 0,"Bad Statement: Missing }");
		next();
		return;
	}
	if(tk == If){
		//TODO: Set Jump/BZ
		next();
		check(tk != '(',"Bad Statement: Missing (");
		next();

		expr(0);

		check(tk != ')',"Bad Statement: Missing )");
		next();

		stmt(); //Deal with brackets
		if(tk == Else){ //if(..){} else
			//TODO: Set Jump/BZ
			next();
			stmt();
		}
	}
	if(tk == While){
		next();
		check(tk != '(',"Bad Statement: Missing (");
		next();
		
		expr(0);

		check(tk != ')',"Bad Statement: Missing )");
		next();

		stmt();
	}
	if(tk == Id){
		printf("id\n");
		expr(Id);
	}
	if(tk == Return){
		printf("Return!\n");
		next();
	}
}

void expr(int lvl){ //tk is at start
	printf("Expression Here! Tk Value: %lld\n", tk);
	next();
}

void glbl(){ //Deal with global variables and func decl.
	while(tk){ //While have token
		check(tk != Int && tk != Char, "Bad Global Declaration: Missing Type");
		type = tk;
		next();
		check(tk != Id, "Bad Global Declaration: Missing ID");
		check(tab[addr].Class == Glo, "Bad Global Declaration: Duplicate");
		next();
		if(tk == ','){ //int a, b, c;
			while(tk != ';'){  
				next(); //a
				check(tk != Id,"Bad Declaration: Missing ID");
				tab[addr].Class = Glo;
				tab[addr].Type  = type;
				next(); //,
			}
		}
		else if(tk == '('){ //Function Start
			//TODO:
			//Change Id.Value to machine code start	
			*cmd = ENT; 
			tab[addr].Value = cmd; //TODO: CORRECT?
			cmd++;

			tab[addr].Class = Fun;
			tab[addr].Type  = type;

			varc = 0;	
			next();
			while(tk != ')' && tk != 0){
				check(tk != Int && tk != Char,"Bad Local Declaration: Missing Type");
				type = tk;
				next(); // Stored/checked ID in table
				check(tk != Id,"Bad Local Declaration: Missing ID");
				check(tab[addr].Class = Loc,"Bad Local Declaration: Duplicate"); //func(int a, int a)

				if(tab[addr].Class){ //If not filled out, fill out
					tab[addr].Type  = type;
					tab[addr].Class = Loc;
					tab[addr].Value = varc;
				}
				else{ //Then there is a global variable w/ same name

					tab[addr].gblType  = tab[addr].Type;
					tab[addr].gblClass = tab[addr].Class;
					tab[addr].gblValue = tab[addr].Value;

					tab[addr].Type  = type; 
					tab[addr].Class = Loc;
					tab[addr].Value = varc;
				}
				varc++;
				next();
				check(tk != ',' && tk != ')',"Bad Syntax: Missing ) or ,");
				if(tk ==',') 
					next();
			}
			//TODO: int func();
			next();
			check(tk != '{', "Bad Function: Missing {");

			next();
			while((tk == Int || tk == Char) && tk != '}') { //If {}, FINISH
				while(tk != ';' && tk != 0){
					type = tk;
					next();
					check(tk != Id,"Bad Declaration: Missing ID");
					check(tab[addr].Class == Loc,"Bad Declaration: Duplicate");

					if(tab[addr].Class){ //If not filled out, fill out
						tab[addr].Type  = type;
						tab[addr].Class = Loc;
						tab[addr].Value = varc;
					}
					else{ //Then there is a global variable w/ same name

						tab[addr].gblType  = tab[addr].Type;
						tab[addr].gblClass = tab[addr].Class;
						tab[addr].gblValue = tab[addr].Value;

						tab[addr].Type  = type; 
						tab[addr].Class = Loc;
						tab[addr].Value = varc;
					}
					varc++; locbp++;
				}
				next();
			}
			*cmd = locbp; cmd++; // Finish ENT
			//ELSE:
			while(tk != '}' && tk != 0){
				stmt();
			}
			check(tk != '}', "Bad Function: Missing }");
			next();
			// Function Done! Prep leaving scope:
			int counter = 0;
			while(tab[counter].Tktype){
				if(tab[counter].gblClass){
						tab[addr].Type   = tab[addr].gblType;
						tab[addr].Class  = tab[addr].gblClass;
						tab[addr].Value  = tab[addr].gblValue;
						//tab[addr].gblType  = tab[addr].gblClass = tab[addr].gblValue = 0; //Probably not necessary?
				}
				counter++;
			}
			*cmd = LEV; cmd++;
		} 
		else if(tk == ';')
			next();
		check(tk != ';',"Bad Declaration: Missing ;");
	}
}
int main(){
	tp = str;
	next();
	glbl();
	return 0;
}
