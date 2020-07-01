//Parser:
//Used to turn code into tokens & beginning the process of converting to machine code.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "compiler.h"

//TODO: Void Type;
//TODO: int a = 3;
//TODO: char support;
//TODO: pointers in function parameters;
//TODO: check pointer referencing;
//            MORE POINTERS THAN POSSIBLE?;
//TODO: delete local variables when leaving scope?;
//TODO: function pointers?;
//TODO: &(*a)


//Get next useful token
// {, (, *, ID, 
void print(){
	while (cnt < cmd) {
		printf("%8.4s", &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
				"OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
				"OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[*cnt++ * 5]);
		if (*(cnt-1) <= ADJ) printf(" %lld\n", *cnt++); else printf("\n");
	}
}
void next(){
	while(tk = *tp++){ // while no useful token found
		if(tk == '}' || tk == '{' || tk == ')' || tk == '(' || tk == ',' || tk == ';') return;
		else if(tk == ' '){
			while(*tp == ' ' && *tp != '0'){
				tp++;
			}
		}
		//Newline
		else if(tk == '\n'){
			ln++;
			printf("%lld: %.*s", ln, tp-lp, lp);
			print();
			lp = tp;
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
			char substr[tp-sstr+1];
			memcpy(substr, &sstr[0], tp-sstr);
			substr[tp-sstr] = '\0';
			//compare
			addr = 0;
			while (tab[addr].Name){
				if((strlen(tab[addr].Name) == tp-sstr) && !memcmp(tab[addr].Name, substr, tp-sstr)){ //if there is an entry, returns 0. !0 = 1
					if(!tab[addr].Tktype) //System call
						tk = Id;
					else
						tk = tab[addr].Tktype;
					return; //addr is in correct spot
				}
				addr++;
			}
			//if done with loop: new variable -> store
			tab[addr].Tktype = Id;
			tab[addr].Name = strdup((char *)substr);
			if(!strcmp((char *)substr,"main"))
				mainaddr = addr;
			tk = Id;
			return;
		}
		else if(tk >= '0' && tk <= '9'){
			val = tk - '0'; //shift ASCII
			while(*tp >= '0' && *tp <= '9')
				val = val*10 + (*tp++ - '0');
			tk = Num;
			return;
		}
		else if(tk == '\"'){
			sstr = tp; // remember start of string "abc"
			while(*tp != '\"' && *tp != 0){
				tp++;  //find end of string	
			}
			check(*tp == 0, "Bad String: Missing ");
			char substr[tp-sstr+1];
			memcpy(substr, &sstr[0], tp-sstr);
			substr[tp-sstr] = '\0';
			gdata = strdup((char *)substr);
			val = (int)gdata;
			gdata += sizeof((char*)substr);
			tk = Str; 
			tp++;
			return;
		}
		else if(tk == '='){ if(*tp == '='){tk = Eq; tp++; return;} else{tk = Assign; return;}}
		else if(tk == '!'){ if(*tp == '='){tk = Ne; tp++; return;} }//TODO: Exclamation mark?
		else if(tk == '+'){ if(*tp == '+'){tk = Inc; tp++; return;} else{tk = Add; return;}}
		else if(tk == '*'){ tk = Mul; return;}
		else if(tk == '/'){ tk = Div; return;}
		else if(tk == '&'){ if(*tp == '&'){tk = Lan; tp++; return;} else{tk = And; return;}}
		else if(tk == '>'){ if(*tp == '='){tk = Ge; tp++; return;} else{tk = Gt; return;}}
		else if(tk == '<'){ if(*tp == '='){tk = Le; tp++; return;} else{tk = Lt; return;}}
	}
}

void check(int expr, char *errmsg){
	//assert(expr);
	if (expr){
		printf("%s\n", errmsg);
		exit(-1);
	}
}

void stmt(){ //tk is at start
	int *addr1, *addr2;
	//Have stmt take care of { and } for easy recursion
	//check(tk != If && tk != Else && tk != While && tk != Return && tk != ';' && tk != '{' && tk != '}' && tk != Id && tk != Mul, "Bad Statement: Expecting keyword/;"); 
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
		next();
		check(tk != '(',"Bad Statement: Missing (");
		next();
		expr(Assign); //Evaluate inside first
		*cmd++ = BZ;
		addr1 = cmd++;
		check(tk != ')',"Bad Statement: Missing )");
		next();

		stmt(); //Deal with brackets
		*addr1 = (int)cmd; //TODO: BZ Prints with address 0
		if(tk == Else){ //if(..){} else
			*addr1 = (int)(cmd + 2); //3 down -> addr -> JMP -> LOC -> ...
			*cmd++ = JMP;
			addr1 = cmd++;
			next();
			stmt();
			*addr1 = (int)cmd;
		}
	}
	//TODO: While with <, > <= >= 
	if(tk == While){
		
		addr1 = cmd; 

		next();
		check(tk != '(',"Bad Statement: Missing (");
		next();
		expr(Assign);
		check(tk != ')',"Bad Statement: Missing )");
		next();

		*cmd++ = BZ;
		addr2 = cmd; //BZ x
		cmd++;	//leave space for address

		stmt();
		*cmd++ = JMP; // BZ x ... JMP
		*cmd++ = (int)addr1; // BZ x ... JMP x 
		*addr2 = (int)cmd; //
	}
	if(tk == Return){
		next();
		expr(Assign);
	}
	else{
		expr(Assign);
	}
}

//Notes:
//-------------------------------------------
//expr Will NEVER parse ; by itself: Done by stmt
//ANY Num, Str, Id etc. DOESNT call PSH by itself: BC it has no context
void expr(int lvl){ //tk is at start
	if(tk == Num || tk == Str){ //LEAF
		next();
		*cmd++ = IMM;
		*cmd++ = val;
	}
	else if(tk == Id){//DOESNT CALL EXPR BY ITSELF: LEAF
		check(!tab[addr].Class,"Undefined Variable");
		next(); //ID can't be followed by another ID: Addr won't move
		//TODO
		if(tab[addr].Class == Sys){
			int func = addr; //Store location
			check(tk != '(',"Bad Function: Missing (");
			next();
			int count = 1; //Should always take variables
			while(tk != ')' && tk != 0){
				expr(Assign);
				*cmd++ = PSH;
				if(tk == ','){
					next();
					count++;
				}
			}
			*cmd++ = tab[func].Value;
			*cmd++ = ADJ;
			*cmd++ = count;
			next();
		}
		else if(tab[addr].Class == Fun){ //Should already be in table: Func declaration can't be inside another function
			int func = addr; //Store location
			check(tk != '(',"Bad Function: Missing (");
			next();

			//Push to stack
			int pin = 0; //count parameters in
			while(tk != ')' && tk != 0){
				expr(Assign); //Use lowest level	
				*cmd++ = PSH; //Everything handled recursively
				pin++;
				if(tk == ',')
					next();
			}
			*cmd++ = JSR;
			*cmd++ = tab[func].Value;
			*cmd++ = ADJ; // move back
			*cmd++ = pin;
			next();
		}
		else if(tab[addr].Class == Loc){
			//Local variables are always stored relatively to the stack.
			//Since it only stays in the stack for its lifetime, it only uses integer values relative to bp and sp location.
			*cmd++ = LEA;
			//TODO: How was this if/else one statement?
			if(tab[addr].Value < parmc) //if parameter
				*cmd++ = 1 + parmc - tab[addr].Value;
			else
				*cmd++ = parmc - 1 - tab[addr].Value;
			*cmd++ = LI;
		}
		else if(tab[addr].Class == Glo){ //Global variables are ALWAYS taken in as a value, unless otherwise changed (i.e. assign)
			*cmd++ = IMM;
			*cmd++ = tab[addr].Value; 
			*cmd++ = LI;
		}
	}
	else if(tk == Mul){ //NEVER Runs operations. If hit, will always be pointer.
		int deref = 0; //number of dereferences
		while(tk == Mul){
			next();
			deref++;
		} //now found the original	
		expr(Mul+1); //let expr handle id
		check(tab[addr].Type < Int, "Bad Variable: Expected Pointer");
		if(tab[addr].Type - Ptr*deref >= Int) // if >, then is pointer, use LI
			while(deref > 0){*cmd++ = LI; deref--;}
		else
			while(deref > 0){*cmd++ = LC; deref--;}
	}
	else if(tk == And){ 
		next();
		expr(And+1);//Should deal with ID
		cmd--;//Just don't do LI
	}

	while(tk >= lvl){ //Precedence Climbing Algorithm //Deal with order of operations
		if(tk == Assign){
			next();
			if(*(cmd-1) == LI)
				*(cmd-1) = PSH;
			else
				*cmd++ = PSH;
			expr(Assign+1);
			*cmd++ = SI;
		}
		else if(tk == Add){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Add+1);
			*cmd++ = ADD;
		}
		else if(tk == Mul){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Mul+1);
			*cmd++ = MUL;
		}
		else if(tk == Div){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Div+1);
			*cmd++ = DIV;
		}
		else if(tk == Eq){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Eq+1);
			*cmd++ = EQ;
		}
		else if(tk == Ne){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Ne+1);
			*cmd++ = NE;
		}
		else if(tk == Le){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Le+1);
			*cmd++ = LE;
		}
		else if(tk == Ge){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Ge+1);
			*cmd++ = GE;
		}
		else if(tk == Gt){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Gt+1);
			*cmd++ = GT;
		}
		else if(tk == Lt){
			next();
			*cmd++ = PSH; //fetch value of first term: a + b
			expr(Lt+1);
			*cmd++ = LT;
		}
	}
}

void glbl(){ //Deal with global variables and func decl.
	check(tk != Int && tk != Char, "Bad Global Declaration: Missing Type");
	type = tk;
	next();
	check(tk != Id && tk != Mul, "Bad Global Declaration: Missing ID/*");
	while(tk == Mul){
		type += Ptr;
		next();
	}
	check(tab[addr].Class == Glo, "Bad Global Declaration: Duplicate");
	next();
	if(tk == ';'){
		tab[addr].Class = Glo;
		tab[addr].Type  = type;
		tab[addr].Value = (int)gdata;
		gdata += sizeof(type);
		next();
	}
	else if(tk == ','){ //int a, b, c;
		tab[addr].Class = Glo;
		tab[addr].Type  = type;
		tab[addr].Value = (int)gdata;
		gdata += sizeof(type); //TODO: type is always integer?
		while(tk != ';'){  
			next(); //a
			check(tk != Id && tk != Mul, "Bad Global Declaration: Missing ID/*");
			type = tk;
			while(tk == Mul){
				type += Ptr;
				next();
			}
			tab[addr].Class = Glo;
			tab[addr].Type  = type;
			tab[addr].Value = (int)gdata;
			gdata += sizeof(type);
			next(); //,
		}
		next();//move off semicolon
	}
	else if(tk == '('){ //Function Start
		//TODO:
		//Change Id.Value to machine code start	
		int func = addr;

		tab[addr].Class = Fun;
		tab[addr].Type  = type;

		varc = 0;	
		parmc= 0;	
		next();
		while(tk != ')' && tk != 0){
			check(tk != Int && tk != Char,"Bad Local Declaration: Missing Type");
			type = tk;
			next(); // Stored/checked ID in table
			check(tk != Id,"Bad Local Declaration: Missing ID");
			check(tab[addr].Class == Loc,"Bad Local Declaration: Duplicate"); //func(int a, int a)

			if(!(tab[addr].Class)){ //If not filled out, fill out
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
			varc++; parmc++;
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
			type = tk;
			next();
			while(tk != ';' && tk != 0){
				check(tk != Id && tk != Mul, "Bad Global Declaration: Missing ID/*");
				while(tk == Mul){
					type += Ptr;
					next();
				}
				
				check(tab[addr].Class == Loc,"Bad Declaration: Duplicate");

				if(!(tab[addr].Class)){ //If not filled out, fill out
					tab[addr].Type  = type;
					tab[addr].Class = Loc;
					tab[addr].Value = varc; //Location in stack
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
				check(tk != ',' && tk != ';',"Bad Syntax: Missing ; or ,");
				if(tk ==',') 
					next();
			}
			next();
		}
		*cmd = ENT; 
		tab[func].Value = (int)cmd++;
		*cmd++ = varc-parmc; // Finish ENT

		//ELSE:
		while(tk != '}' && tk != 0){
			stmt();
		}
		check(tk != '}', "Bad Function: Missing }");
		next();
		// Function Done! Prep leaving scope:
		int counter = 0;
		while(tab[counter].Name){
			if(tab[counter].gblClass){
				tab[addr].Type   = tab[addr].gblType;
				tab[addr].Class  = tab[addr].gblClass;
				tab[addr].Value  = tab[addr].gblValue;
				//tab[addr].gblType  = tab[addr].gblClass = tab[addr].gblValue = 0; //Probably not necessary?
			}
			counter++;
		}
		if(mainaddr == func)
			*cmd++ = EXIT;
		else
			*cmd++ = LEV;
	} 
}
