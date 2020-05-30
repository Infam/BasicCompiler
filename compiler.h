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

struct table{ //symbol table
	int Tktype; 
	char* Name; 
	int Class;
	int Type;
	int Value;
	int gblClass;
	int gblType;
	int gblValue; //in the case of a global declaration w/ same name
};


// File
extern int fd; //File descriptor

// Debug
extern char *lp; //Line Pointer
extern int ln; //Line Pointer
extern int *cnt; //count of command: used to print commands

// Pointers
extern int *sp, *bp, *pc, *cmd, *sysc; //stack pointers, and machine code pointers
extern int eax, inst; // eax register, inst = (pc -1)

extern int addr1, addr2;

//Actual text
extern char *str; 
extern char *sstr; //Start of string
extern char *tp; //text pointer

//Symbol table & Next call:
//tk will hold both characters and integer values
extern int tk; //the token type
extern int addr; //will hold index of wanted item
extern int mainaddr; //Holds address of main function
extern int type; // token type: int abc
extern int val; //value of found integers, i.e. int a = 20
extern int parmc; //function parameter count
extern int varc; //Local Variable Counter

extern char * gdata; //Global data area & strings

extern struct table tab[DATASZ];

void next();
void glbl();
void check();
void stmt();
void expr();
void print();
