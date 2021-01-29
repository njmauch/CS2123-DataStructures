/**********************************************************************
cs2123p1.h
Copyright 2019 Larry Clark, this code may not be copied to any other website
Purpose:
   Defines constants:
	   max constants
	   error constants
	   warning constants
	   categories of tokens (operator, operand, etc.)
	   boolean constants
   Defines typedef for
	   Token
	   Element  (values placed in stack or out)
	   StackImp (array stack implementation)
	   Stack    (pointer to a StackImp)
	   OutImp   (out implementation)
	   Out      (pointer to an OutImp)
Notes:

**********************************************************************/
/*** constants ***/
// Maximum constants
#define MAX_STACK_ELEM 20       // Maximum number of elements in the stack array
#define MAX_TOKEN 50            // Maximum number of actual characters for a token
#define MAX_OUT_ITEM 50         // Maximum number of Out items

// Error constants (program exit values)
#define ERR_EXIT 999             // Unexpected error

// Warning constants.  Warnings do not cause the program to exit.
#define WARN_MISSING_RPAREN   801
#define WARN_MISSING_LPAREN   802
#define WARN_MISSING_OPERATOR 803
#define WARN_MISSING_OPERAND  804

// categories of tokens
#define CAT_LPAREN 1        // (
#define CAT_RPAREN 2        // )
#define CAT_OPERATOR 3      // Operators are =, NEVER, ONLY, AND, OR
#define CAT_OPERAND 4       // These are trait types and trait values

// boolean constants
#define FALSE 0
#define TRUE 1

/*** typedef ***/

// Token typedef used for operators, operands, and parentheses
typedef char Token[MAX_TOKEN + 1];

// Element typedef used for Element values placed in the stack or out
typedef struct
{
	Token szToken;      // Could be a variable, numeric constant, operator,
						// left parenthesis or right parenthesis
	int iCategory;      // A value used to make it easier to handle
						// different cases for the types of tokens
	int iPrecedence;    // An integer representing the operator 
						// precedence.  Higher values imply 
						// higher precedence.
} Element;

// StackImp typedef defines how we implement a stack using an array
typedef struct
{
	int iCount;  // number of elements in stack.  0 is empty 
	Element stackElementM[MAX_STACK_ELEM];
} StackImp;

// Stack typedef defines a pointer to a stack
typedef StackImp *Stack;

// OutImp typedef defines how we implement out
typedef struct
{
	int iOutCount;
	Element outM[MAX_OUT_ITEM];
} OutImp;

// Out typedef defines a pointer to out
typedef OutImp *Out;

/**********   prototypes ***********/

// Stack functions

void push(Stack stack, Element value);
Element pop(Stack stack);
int isEmpty(Stack stack);
Stack newStack();
void freeStack(Stack stack);
Element topElement(Stack stack);

// Conversion to Postfix functions that Larry provided
void categorize(Element *pElement);
void addOut(Out out, Element element);
void printOut(Out out);

// Conversion to Postfix functions that each student must implement
int convertToPostfix(char *pszInfix, Out out);

// Utility routines
void errExit(char szFmt[], ...);
char * getToken(char *pszInputTxt, char szToken[], int iTokenSize);