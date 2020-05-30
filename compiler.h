#define int long long //So converting from int pointer to int has no issues
#define DATASZ 1000
//------------------------------------
//Questions:
//------------------------------------
//Why cmd and pc?
//Why use pointers over values?
//Use Hashing to reduce search time?
//Why still linearly search through arrays then?

//machine code
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
	OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
	OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

//Starting enum from 128:
enum {
	Num = 128, Fun, Sys, Glo, Loc, Id, Str,
	Char, Else, Enum, If, Int, Return, Sizeof, While, Printf, 
	Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// File
int fd; //File descriptor


// Debug
char *lp; //Line Pointer
int ln; //Line Pointer
int *cnt; //count of command: used to print commands

// Pointers
int *sp, *bp, *pc, *cmd, *sysc; //stack pointers, and machine code pointers
int eax, inst; // eax register, inst = (pc -1)

int addr1, addr2;
//Actual text
char *str; 
char *sstr; //Start of string
char *tp; //text pointer

//Symbol table & Next call:
//tk will hold both characters and integer values
int tk; //the token type
int addr; //will hold index of wanted item
int type; // token type: int abc
int val; //value of found integers, i.e. int a = 20
int parmc; //function parameter count
int varc; //Local Variable Counter

char * gdata; //Global data area & strings
struct table{ //symbol table
	int Tktype; char* Name; int Class, Type, Value;

	int gblClass, gblType, gblValue; //in the case of a global declaration w/ same name
} tab[DATASZ];


void next();
void glbl();
void check();
void stmt();
void expr();
