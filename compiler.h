#define int long long //So converting from int pointer to int has no issues

//machine code
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
	OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
	OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

//Starting enum from 128:
enum {
	Num = 128, Fun, Sys, Glo, Loc, Id,
	Char, Else, Enum, If, Int, Return, Sizeof, While,
	Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// Pointers
int *sp, *bp, *pc, *cmd; //stack pointers, and machine code pointers
int locbp = 2; //local base pointer offset: Only counts parameters. +2 for pc, return point
int eax, inst; // eax register, inst = (pc -1)

//Actual text
char *str; 
char *sstr; //Start of string
char *tp; //text pointer

//Symbol table & Next call:
//tk will hold both characters and integer values
int tk; //the token type
int addr = 0; //will hold index of wanted item
int type; // token type: int abc
int val; //value of found integers, i.e. int a = 20
int varc; //Local Variable Counter

struct tab{ //symbol table
	int Tktype; char* Name; int Class, Type, Value;

	int gblClass, gblType, gblValue; //in the case of a global declaration w/ same name
} 

tab[100] = {
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

void next();
void glbl();
void check();
void stmt();
void expr();

//------------------------------------
//Questions:
//------------------------------------
//Why cmd and pc?
//Why use pointers over values?
//Use Hashing to reduce search time?
//Why still linearly search through arrays then?
