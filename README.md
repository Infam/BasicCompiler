A less versatile version of [rswier's c4 compiler](https://github.com/rswier/c4) written for educational purposes only.  
I used rswier's code as a guide to learning the logic behind compilers and how they work.  
After walking through a portion of rswier's code, I would try and write it by myself to solidify my understanding. Unlike rswier's code, much of mine is left commented in case I ever want to review or look back at it.

While the majority of code was written by me to understand the logic, the general layout, and print function were all done by rswier.

The following is my understanding/documentation of how the compiler works.

# Overview

This C compiler is written primarily to understand how compilers work. This means there is a lot of simplification both in the amount of features covered and how instructions work/are processed.

The basic layout can be thought of as the following layers:

## Lexer

Reads in the code character by character and splits the code into basic units called tokens

## Parser

I feel that parser is better explained as being made up of grammar and the actual parsing itself.
However, these two are pretty much blended together when coding, since if a grammar rule is followed, we know how to process a pattern of tokens almost immediately.

## Grammar

Just like in any language, establishes the rules around how words (in this case tokens) are placed.
In this case we are writing grammar based on C's syntax rules.
Uses the pattern of tokens to check whether the rules are followed or not.

## Actual Parsing

If the rules are followed, we do certain actions based on the pattern of tokens.
This includes storing variables and eventually figuring out the actual machine instructions/assembly code.

It is the parser's job to fill out the symbol table (tab[]) and list of commands (cmd), as well as populate the data section (gdata).

## Virtual Machine (vm.c)

Processes the machine instructions/ runs the code.

# Layout

This layout completely skips the assembly step of converting assembly code into machine code and opts to use a virtual machine for simplicity.
Many of the instructions will also be different from typical x86 to simplify understanding by combining calls together, or are simply omitted altogether.

As of 6/29/2020 I have code oriented in the following fashion:

### parser.c

Contains both the lexer and parser portion of the compiler.

**print():**

Prints out the lines in code.c and the associated assembly code generated.  
Note that BZ doesn't display the address correctly both in mine and rswier's versions, since it is printed out before the address is actually changed.

**next():**

Walks through code.c and returns the next useful token.
Note that next stops at the first character of the next useful token.
Also notes the location of the main function

**check(int expr, char \*errmsg):**

Checks incoming statement and stops compiler if wrong. Used to simplify reading code.

**stmt():**

Parses statements

**expr(int lvl):**

Parses expressions. Very special. (See [Basic Logic](#basic-logic))

**glbl():**

Figures out everything global. Since everything must start as global, this is the entry point of the parser.

### vm.c

Contains the virtual machine and all necessary variables

**main():**

Contains the main function, which endlessly runs a while loop and exits when it sees the EXIT instruction.

### compiler.h

Contains all variables used by both parser.c and vm.c.

### code.c

The code that the compiler actually compiles.  
All the functions have a return type of void, because they never need to pass values back to one another.

# Simplifications

- x86 Instructions are simplified, combined, or just removed
- No struct keyword support
- No preprocessor support
- No typedef
- No for loop support
- No Switch statement support
- No Void Type (As of 6/29/2020)
- No in-line assignment (int a = 3; -> int a; a = 3;)
- Only simulate one register (eax)

# Basic Logic

The order these functions are executed is always Globals -> Statements -> Expressions.

## Globals

Globals are any code that runs are located in the outermost scope of a C file.

**include/define:**

- Skipped over, out of focus of learning the basics of the compiler

**//comments:**

- Skipped over, as they should be

**Global Variables:**

- added to symbol table (see [Patterns](#patterns))

**functions():**

- added to symbol table
- Calls stmt() after **{** (see [Patterns](#patterns))

## Statements

Anything inside a function, but not an expression.  
Anything in brackets will recursively call stmt() (i.e. if(){}).

**Local Variables:**

- added to symbol table

**if/else statment:**

- Calls expr(int Assign) after **(** (see [Patterns](#patterns))

**while statement:**

- Calls expr(int Assign) after **(** (see [Patterns](#patterns))

**return statement:**

- Calls expr(int Assign) immediately (see [Patterns](#patterns))

## Expressions

Expressions use the Precedence Climbing Algorithm to ensure that operations are done in the right order.  
Everytime expression is called it is given a level that corresponds to the order of operations enum table (see [compiler.h](#compilerh)).  
expr() always runs until the whole expression is calculated, and only then does the scope change back to stmt().  
Entering expression always starts with the lowest level with the lowest integer value, Assign. Inside expr() it can recursively call itself with higher level values. This means that expr knows the previous expression that happened before it.

expr() is logically set up to create a tree:

**Leaves:**
expr() always ends with a while loop that is run every time it is entered. This loop runs until the token scanned is smaller than the input level. Since expr always enters with Assign, the while loop only stops when tk < Assign, which is only when tk is a Num or Id. This logically makes those leaf nodes (see [compiler.h](compilerh))

- Number:
  Used as direct value
- id/Pointers/Addresses:
  Parsed to get its value and get ready to use it. Can also be a system call. (See Variables)
- System Calls:
  System calls are keywords included in the symbol table. When hit, expr() loads all parameters into the stack. The real system call is then called with all the parameters loaded in.

**Nodes:**

- Operations:
  Add, Subtract, etc.
- Logical Operators: >, <, >=, ==

**_expr() uses LR parsing which translates the expression into Reverse Polish Notation through post-order traversal._**  
Post-order traversal (Left Right Root) is achieved through the layout of the function as follows:

```
    expr(int lvl):
	Leaves
	while (tk >= lvl):
	    if tk = ... :
		expr(tk + 1)
		Nodes
```

Leaves are always added as soon as expr is entered, while Nodes are only touched when a loop has finished. In other words the left and right are visited first before the root, which is post-order traversal.  
Calculations are only done once we have reached a point where the following operation is of a lower level than our current. We then roll back and calculate until the level is lower than that if the following operation. (See [Expression Example](#expression-example))  
Logical operators also have a place in the heirarchy. (See [compiler.h](#compilerh))

# Variables

**Local:**
Local variables are stored in the symbol table which holds their Tktype(id, system call), Type(int, char), Name(actual string), Class(Loc, Glo), and Value(offset/variable count (varc)).  
Depending on their Value, a function can determine whether or not it is a parameter, as parameters will be above the return points, whereas actual local variables will be below.  
Since local variables are only alive as long as the function's scope doesn't increase (i.e. don't return), they are stored in the stack.  
They can therefore be accessed using a calculted offset, instead of their actual memory address (See [LEA/Patterns](#patterns)).

**Global:**
Global variables are stored similarly to local variables, but their value is the address in the data section of process (gdata) instead of an offset. (See [Patterns](#patterns))

**Duplicates/Shadowing:**
If an id is used globally and also locally, the global declaration is moved to gblType, gblClass, gblValue and the local is put in its place. Once the scope exits the function, then everything is moved back. Since the symbol table is searched through and not pointed to, moving items around doesn't create any issues.

# Pointers

Pointers are sort of special so I wanted to give them their own section.  
Only the most basic implementation of pointers is handled here (i.e. setting and getting values).

### Notes

Just some basic notes, since I find pointers confusing.

**Single Pointers:**

- if `a` is declared as `int *a`:
- `a`, which is of type `int *`, will be the address that it points to
- `*a`, which is of type `int * - * = int`, will be the contents pointed to
- `&a`, which is of type `int * + * = int **`, will be the address of the pointer itself.

**Addresses:**

- From the example above, adding an `&` creates another level of indirection and is the same as increasing the type by a `*`.
- `&` always gives you the address of the thing itself. So `&&a` is not a thing (thankfully).

**Assignments:**

**_Since assigning never has a_** `*` **_, it is treated as an id assignment. Assigning will never have any_** `*` **_'s, as that is assigning to what a pointer points to, which at the beginning is junk. Pointers therefore must be assigned before being used:_**

```
    int *a; int b;
    *a = b //a's type of int * - * = int and b's type of int + * = int * do match.
           //The contents of what a points to is given the value of b, but a is pointing to junk that might not have write permissions or may be overwritten later.
	   //This usually results in a Segmentation Fault.
    VS:
    a = &b; //a's type of int * and b's type of int + * = int * match.
	    //What a points to is correctly assigned the address of b.
```

***On top of this, *a would also correctly give the value of b.\***

**Double Pointers:**

Just like with single pointers, every pointer must be assigned, and the types must be right.  
**_The compiler doesn't automatically assign two spaces for double pointers to point correctly._**

```
    int **a; int \*b; int c;
    a = &b; //a's type of int ** matches &b's type of int * + *
    b = &c; //b's type of int * matches &c's type of int + *
    //*a is b
    //\*\*a = *(b) is c
```

**expr:**

Since `*` is used both as a math operation and for pointers, expr() has the responsibility of differentiating between the two.  
This is solved easily as every time expr() is entered, it should always start on a leaf, since every operation calls a next(). This means that if the token is a _ when entering expr(), we know for sure that we are dealing with pointers.  
Unless there is special notation (i.e. `_=`, which is handled as a unit), a multiplication operation always happens between two variables/numbers. Therefore the first multiplication symbol after an id must be a math operation. Since expr() executes using this alteration between id's/numbers and operations (or leafs and nodes), expr() will always be able to tell if a`\*` is associated with a pointer or not.

**Calculation:**

When declaring pointers, the number of indirections (number of stars) is noted by adding Ptr a similar number of times to the id's type.  
By subtracting and adding Ptr's value we know how many indirections as well as the base type of the pointer. (See Pointer Calculation Example)  
**_Note that checking for pointer in expr() means I am processing/calculating the value._** When using a pointer, I do not necessarily have to dereference all the way to the bottom.

- Assignments:
  - Assignments don't need anything extra. since variables all have addresses we just need to fetch the address. (See [Patterns](#patterns))
- Single Pointer: `*a`
  - call expr() id portion, then run LI
- Multiple Pointers: `**a`
  - call expr() id portion, call LI for as many dereferences as there are.
  - Calling LI after LEA gets the contents at the address in eax. By doing this multiple times we step through the pointers until we get to the right location.

# Virtual Machine

The machine allocates space for the following:

- Stack, which is pointed to/walked through by sp and bp
- gdata, Global data area, which is walked through just using gdata as a pointer
- cmd, which is first added to by cmd as a pointer, and walked through by pc
- Text, which is pointed to/walked through by tp and tk

After parsing everything, the symbol table becomes irrelevant and the code gets ready to run.  
pc is set to where the main function is called and starts walking through code  
The virtual machine then runs all the commands and exits when it sees EXIT.

# Instructions

Here is an explanation of the currently supported instructions.

**Scope Changing:**
These instructions deal with entering and leaving functions. Any instruction with round brackets takes two spaces.

- ENT(# params):
  - Save the bp to the stack, so we know where to return to
  - Move the sp to the same position
  - Leave space for the number of parameters coming in
- JSR:
  - Jump to Sub-Routine. Save pc and jump to new location.
- LEV:
  - Move the sp back to bp
  - Set bp back to the previous frame
  - Place the pc back to where it used to be

**Fetching:**

- LEA(variable offset):
  - Load Effective Address.
  - Loads the address of a variable into eax.
- IMM(value to load):
  - Loads preceding value into eax.
- PSH:
  - Places eax value into the stack.
- LI:
  - Load integer. Loads integer value from address in eax.
- SI:
  - Store Integer. Stores integer value to an address located in stack.

**Flow:**

- JMP(address to jump to):
  - Change pc to be the address that it's currently pointed to.
- BZ(address to jump to):
  - Branch (if) Zero. If eax = 1, continue. Else, jump.
- BNZ(address to jump to):
  - Branch (if) Not Zero. Opposite of BZ.
- ADJ(Variable count):
  - Adjust sp so it overrides parameters when calling functions.
- EXIT:
  - Exits the whole program.

**System Calls:**

- PRTF(Uses ADJ's count. See Patterns):

  - Calls the actual printf() system call, with parameters in the stack.

**Operations:**

- ADD:
  - `+` value in stack and eax together. Put in Stack
- SUB:
  - `-` value in stack and eax together. Put in Stack
- MUL:
  - `*` value in stack and eax together. Put in Stack
- DIV:
  - `/` value in stack and eax together. Put in Stack
- EQ:
  - Check if top of stack and eax are equal. Place 1 or 0 in eax.
- NE:
  - Check if top of stack and eax are not equal. Place result in eax.
- GE:
  - Check if top of stack is greater or equal to eax. Place result in eax.
- LE:
  - Check if top of stack is less than or equal to eax. Place result in eax.
- GT:
  - Check if top of stack is greater than eax. Place result in eax.
- LT:
  - Check if top of stack is less than eax. Place result in eax.

# Patterns

These patterns are always followed regardless of how the code is written; The steps to enter a function are always the same, etc.

**Load Local Variable value:**

    - LEA (offset) -> LI
    - Gets value of Local Variable from it's address in stack.

**Load Global Variable value:**

    - IMM (addrs) -> LI
    - Gets value of Global Variable from it's address in gdata.

**Execute System Call:**

    - PRTF -> ADJ (param count)
    - Call PRTF with param count number. Then move sp to erase all parameters.

**Entering Function:**

    - ENT (param count)
    - Runs ENT. (See Instructions)

**Leaving Function:**

    - LEV -> ADJ (Entered param count)
    - Runs LEV to shift bp and pc, and ADJ to move sp.

# Pointer Calculation Example

**Declaration:**

```
    int **a; //a.Type = Int + Ptr + Ptr
    int *b; //b.Type = Int + Ptr
    int c;
```

**Assignment:**

```
    a = &b; //a's value in stack is given b's stack address (assuming local).
    b = &c; //b's value in stack is given c's stack address (assuming local).
    c = 5; //c is given value of 5.
```

**Usage:**

```
    **a ...;
    type = a.Type - Ptr - Ptr = Int
```

# Expression Example

For example, in the string `1 + 3 * 4 + 5`, Mul has a value of 162 and Add a value of 160.  
The tree would look like so:

```
         +
        / \
       +   5
      / \
     1   *
        / \
       3   4
```

If the compiler is LR, the output should agree with Post-Order Traversal of the tree, which would give `1 3 4 * + 5 +`.

**1st expr:**

First expr will parse 1 -> + and enter the first while loop.

```
    string: 1 + 3 * 4 + 5;
    1st While loop:
    	tk = Add, lvl = Assign
    	PSH instruction
    	Call 2nd expr(Add+1 = 161 = Sub)

    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
```

**2nd expr:**

Second expr will parse 3 -> \* and enter the second while loop.

```
    string: 3 * 4 + 5;
    2nd While loop:
	tk = Mul, lvl = Sub
	PSH instruction
	Call 3rd expr(Sub+1 = 162 = Mul)
    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    3     |
	+----------+
	|   PSH    |
	+----------+
```

**3rd expr:**

Third expr will parse 4 -> + but stop at the while loop.

```
    string: 4 + 5;
    3rd While loop:
	tk = Add, lvl = Mul
	STOP. back to 2nd while loop
    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    3     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    4     |
	+----------+
```

**2nd While loop:**

Second while loop now has tk = +. It therefore adds its instruction and stops, as Add(160) is less than Sub(161).

```
    string: 5;
    tk = Add, lvl = Sub
    MUL instruction
    STOP. back to 1st While loop
    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    3     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    4     |
	+----------+
	|   MUL    |
	+----------+
```

**1st While loop:**

First while loop still has tk = +. Since this is still greater than the lvl of Assign, we run it again.

```
    string: 5;
    tk = Add, lvl = Assign
    ADD instruction
    Loop Again:
	Call 4th expr(Add+1 = 161 = Sub)
    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    3     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    4     |
	+----------+
	|   MUL    |
	+----------+
	|   ADD    |
	+----------+
```

**4th expr:**

Fourth expr will parse 5 and stop, since Id, or Num, are lower numbers in the enum. (see [compiler.h](#compilerh))

```
    string: ;
    4th While loop:
	STOP. back to 1st while loop
	tk = Num, lvl = Sub
    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    3     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    4     |
	+----------+
	|   MUL    |
	+----------+
	|   ADD    |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    5     |
	+----------+
```

**1st expr:**

First expr will finally stop, since Num is less than Assign.

```
    string: ;
    1st While loop:
	tk = Num, lvl = Assign
	ADD instruction
	Finish.
    Cmd:
	+----------+
	|   IMM    |
	+----------+
	|    1     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    3     |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    4     |
	+----------+
	|   MUL    |
	+----------+
	|   ADD    |
	+----------+
	|   PSH    |
	+----------+
	|   IMM    |
	+----------+
	|    5     |
	+----------+
	|   ADD    |
	+----------+
```

Since cmd is run from top to bottom, this would be run as `1 3 4 * + 5 +`. This therefore confirms that the compiler uses LR parsing, as well as reverse polish notation.
