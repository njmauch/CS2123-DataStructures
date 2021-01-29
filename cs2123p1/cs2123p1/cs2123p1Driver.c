/******************************************************************************
cs2123p1Driver.c by Larry Clark
Copyright 2019 Larry Clark, this code may not be copied to any other website
Purpose:
	This program reads queries expressions in infix notation and converts them
	to postfix using a stack.
Command Parameters:
	n/a
Input:
	The standard input file stream contains queries (one per input text line).
	Tokens in the query will be separated by one space.
	Some sample data:
		SMOKING = N AND GENDER = F
		SMOKING = N AND ( EXERCISE = HIKE OR EXERCISE = BIKE )
		GENDER = F AND EXERCISE NEVER YOGA
		SMOKING = N AND EXERCISE = HIKE OR EXERCISE = BIKE
		( BOOK = SCIFI )
		( ( ( BOOK ONLY SCIFI ) ) )
Results:
	For each query, print the query and its corresponding postfix expression.
Returns:
	0 - normal
	999 - algorithm error (see message for details)
Notes:
	1. This program uses an array to implement the stack.  It has a maximum of
	   MAX_STACK_ELEM elements.
	2. This program uses an Out array for the resulting postfix expression.
	   It has a maximum of MAX_OUT_ITEM elements.
*******************************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p1.h"

// The following structure is used by the categorize function to help
// categorize tokens and provide the precedence.   
static struct
{
	char szSymbol[MAX_TOKEN + 1];   // token symbol such as "("
	int iCategory;                  // CAT_OPERATOR, CAT_OPERAND, CAT_LPAREN or CAT_RPAREN
	int iPrecedence;                // 0 - lowest precedence, 3 - highest precedence
} symbolDefM[] =
{
	  {"(",      CAT_LPAREN,   0}
	, {")",      CAT_RPAREN,   0}
	, {"=",      CAT_OPERATOR, 2}
	, {"NEVER",  CAT_OPERATOR, 2}
	, {"ONLY",   CAT_OPERATOR, 2}
	, {"AND",    CAT_OPERATOR, 1}
	, {"OR",     CAT_OPERATOR, 1}
	, {"", 0, 0}
};

// Stack implementation using arrays.  You are not required to document these.
void push(Stack stack, Element value)
{
	if (stack->iCount >= MAX_STACK_ELEM)
		errExit("Attempt to PUSH more than %d values on the array stack"
			, MAX_STACK_ELEM);
	else
	{
		stack->stackElementM[stack->iCount] = value;
		stack->iCount++;
	}
}
Element pop(Stack stack)
{
	if (isEmpty(stack))
		errExit("Attempt to POP an empty array stack");
	else
	{
		stack->iCount--;
		return stack->stackElementM[stack->iCount];
	}
}
Element topElement(Stack stack)
{
	if (isEmpty(stack))
		errExit("Attempt to examine topElement of an empty array stack");
	else
		return stack->stackElementM[stack->iCount - 1];    // return the top
}
int isEmpty(Stack stack)
{
	return stack->iCount <= 0;
}
Stack newStack()
{
	Stack stack = (Stack)malloc(sizeof(StackImp));
	stack->iCount = 0;
	return stack;
}
void freeStack(Stack stack)
{
	free(stack);
}


// Main program for the driver

int main(int argc, char *argv[])
{
	Out out = malloc(sizeof(OutImp));
	char szInputBuffer[100];    // entire input line
	int rc;
	int iLineCount = 0;

	// read text lines containing queries until EOF
	while (fgets(szInputBuffer, 100, stdin) != NULL)
	{
		iLineCount++;
		// Ignore the line if it only contains a linefeed
		if (szInputBuffer[0] == '\n')
			continue;
		printf("Query # %d: %s", iLineCount, szInputBuffer);

		out->iOutCount = 0;   // reset out to empty
		rc = convertToPostfix(szInputBuffer, out);
		switch (rc)
		{
		case 0:
			printOut(out);
			break;
		case WARN_MISSING_LPAREN:
			printf("\tWarning: missing left parenthesis\n");
			break;
		case WARN_MISSING_RPAREN:
			printf("\tWarning: missing right parenthesis\n");
			break;
		case WARN_MISSING_OPERATOR:
			printf("\tWarning: missing operator\n");
			break;
		case WARN_MISSING_OPERAND:
			printf("\tWarning: missing operand\n");
			break;
		default:
			printf("\t warning = %d\n", rc);
		}
	}
	free(out);
	printf("\n");
	fclose(stdin);
	fclose(stdout);
	return 0;
}

/******************** addOut **************************************
void addOut(Out out, Element element)
Purpose:
	Adds an element to out.
Parameters:
	I/O Out out                 Stores the postfix expression
	I Element element           Element structure to be added to out.
Returns:
	n/a
Notes:
	- Since out uses an array, addOut does a boundary check for overflow.
**************************************************************************/
void addOut(Out out, Element element)
{
	if (out->iOutCount >= MAX_OUT_ITEM)
		errExit("Overflow in the out array");
	out->outM[out->iOutCount++] = element;
}

/******************** printOut **************************************
void printOut(Out out)
Purpose:
	prints the contents of the out array to stdout
Parameters:
	I Out out                 The postfx expression
Returns:
	n/a
Notes:
	- Prints 12 tokens from out per line
**************************************************************************/
void printOut(Out out)
{
	int i;
	printf("\t");
	// loop through each element in the out array
	for (i = 0; i < out->iOutCount; i++)
	{
		printf("%s ", out->outM[i].szToken);
		if (((i + 1) % 12) == 0)
			printf("\n\t");
	}
	printf("\n");
}

/******************** categorize **************************************
void categorize(Element *pElement)
Purpose:
	Categorizes a token providing its precedence (0 is low, higher
	integers are a higher precedence) and category (operator, operand,
	left paren, right paren).  Since the category is an integer, it can
	be used in a switch statement.
Parameters:
	I/O Element *pElement       pointer to an element structure which
								will be modified by this function
Returns:
	n/a
Notes:
	- The input parameter, pElement must point to an existing Element
	  structure.  Its attribute, szToken, must already be populated.
	- This function will populate the iCategory and
	  iPrecedence values in the Element structure referenced by pElement.
	- Uses the symbolDefM array to help categorize tokens
**************************************************************************/
void categorize(Element *pElement)
{
	int i;
	// loop through the symbolDefM array until an empty symbol value is found
	// marking the end of the symbolDefM array
	for (i = 0; symbolDefM[i].szSymbol[0] != '\0'; i++)
	{
		// does the element's token match the symbol in the symbolDefM array?
		if (strcmp(pElement->szToken, symbolDefM[i].szSymbol) == 0)
		{   // matched, so use its precedence and category
			pElement->iPrecedence = symbolDefM[i].iPrecedence;
			pElement->iCategory = symbolDefM[i].iCategory;
			return;
		}
	}
	// must be an operand
	pElement->iPrecedence = 0;
	pElement->iCategory = CAT_OPERAND;
}

/******************** getToken **************************************
char * getToken (char *pszInputTxt, char szToken[], int iTokenSize)
Purpose:
	Examines the input text to return the next token.  It also
	returns the position in the text after that token.
Parameters:
	I   char *pszInputTxt       input buffer to be parsed
	O   char szToken[]          Returned token.
	I   int iTokenSize          The size of the token variable.  This is used
								to prevent overwriting memory.  The size
								should be the memory size minus 1 (for
								the zero byte).
Returns:
	Functionally:
		 Pointer to the next character following the delimiter after the token.
		 NULL - no token found.
	szToken parm - the returned token.  If not found, it will be an
		 empty string.
Notes:
	- If the token is larger than the szToken parm, we return a truncated value.
	- If a token isn't found, szToken is set to an empty string
	- This function does not skip over white space occurring prior to the token.
	- If the input buffer pointer is NULL, the function terminates with
	  an algorithm error.
**************************************************************************/
char * getToken(char *pszInputTxt, char szToken[], int iTokenSize)
{
	int iDelimPos;                      // found position of delim
	int iCopy;                          // number of characters to copy
	char szDelims[20] = " \n\r";        // delimiters
	szToken[0] = '\0';

	// check for NULL pointer 
	if (pszInputTxt == NULL)
		errExit("getToken passed a NULL pointer");

	// Check for no token if at zero byte
	if (*pszInputTxt == '\0')
		return NULL;

	// get the position of the first delim
	iDelimPos = strcspn(pszInputTxt, szDelims);

	// See if the token has no characters before delim
	if (iDelimPos == 0)
		return NULL;

	// see if we have more characters than target token, if so, trunc
	if (iDelimPos > iTokenSize)
		iCopy = iTokenSize;             // truncated size
	else
		iCopy = iDelimPos;

	// copy the token into the target token variable
	memcpy(szToken, pszInputTxt, iCopy);
	szToken[iCopy] = '\0';              // null terminate

	// advance the position
	pszInputTxt += iDelimPos;
	if (*pszInputTxt == '\0')
		return pszInputTxt;
	else
		return pszInputTxt + 1;
}
/******************** errExit **************************************
	void errExit(char szFmt[], ... )
Purpose:
	Prints an error message defined by the printf-like szFmt and the
	corresponding arguments to that function.  The number of
	arguments after szFmt varies dependent on the format codes in
	szFmt.
	It also exits the program, returning ERR_EXIT.
Parameters:
	I   char szFmt[]            This contains the message to be printed
								and format codes (just like printf) for
								values that we want to print.
	I   ...                     A variable-number of additional arguments
								which correspond to what is needed
								by the format codes in szFmt.
Returns:
	Returns a program exit return code:  the value of ERR_EXIT.
Notes:
	- Prints "ERROR: " followed by the formatted error message specified
	  in szFmt.
	- Prints the file path and file name of the program having the error.
	  This is the file that contains this routine.
	- Requires including <stdarg.h>
**************************************************************************/
void errExit(char szFmt[], ...)
{
	va_list args;               // This is the standard C variable argument list type
	va_start(args, szFmt);      // This tells the compiler where the variable arguments
								// begins.  They begin after szFmt.
	printf("ERROR: ");
	vprintf(szFmt, args);       // vprintf receives a printf format string and  a
								// va_list argument
	va_end(args);               // let the C environment know we are finished with the
								// va_list argument
	printf("\n\tEncountered in file %s\n", __FILE__);  // this 2nd arg is filled in by
								// the pre-compiler
	exit(ERR_EXIT);
}
