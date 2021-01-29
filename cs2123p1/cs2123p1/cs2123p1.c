/******************convertToPostfix********************************
cs2123p1.c by Nathan Mauch
int convertToPostfix(char *pszInfix, Out out)
Purpose:
	Converts an expression from infix notation into
	postfix notation.
Paramaters:
	I	char *pszInfix	Expression in infix notation to get tokens from
	I/O	Out out			Stores the postfix expression
Notes: 
	1. This program uses an array to gather tokens to put into a stack and then
	an Out array to store the postfix notation expression.
Return value:
	WARN_MISSING_RPAREN		The infix expression is missing a right parenthesis
	WARN_MISSING_LPAREN		The infix expression is missing a left parenthesis
	0						The expression is normal
*****************************************************************/
#include "cs2123p1.h"

int convertToPostfix(char *pszInfix, Out out)
{
	char szToken[MAX_TOKEN];	//stores token 
	Stack stack = newStack();
	Element element;
	Element popped;
	int bFoundParen;	//Boolean for if parenthesis is found or not

	pszInfix = getToken(pszInfix, szToken, MAX_TOKEN);
	//gets tokens from input document while document isn't empty
	while (pszInfix != '\0')
	{
		//stores token in elemnent and categorizes element
		strcpy(element.szToken, szToken);
		categorize(&element);

		//compares category of each token
		switch (element.iCategory)
		{
		case CAT_OPERAND:
			addOut(out, element);
			break;
		case CAT_OPERATOR:
			//check if stack is empty
			while (isEmpty(stack) == FALSE)
			{
				//if not empty gets the top value on the stack
				popped = topElement(stack);
				//checks the precendence of the current token to the top of the stack
				if (element.iPrecedence > popped.iPrecedence)
				{
					break;
				}
				addOut(out, pop(stack));
			}
			push(stack, element);
			break;
		case CAT_LPAREN:
			push(stack, element);
			break;
		case CAT_RPAREN:
			//sets boolean value
			bFoundParen = FALSE;
			while (isEmpty(stack) == FALSE)
			{
				popped = pop(stack);
				//checking to find left parenthesis that belongs with right paren
				if (popped.iCategory == CAT_LPAREN)
				{
					bFoundParen = TRUE;
					break;
				}
				addOut(out, popped);
			}
			//if never finds left paren, returns error
			if (bFoundParen == FALSE)
			{
				freeStack(stack);
				return WARN_MISSING_LPAREN;
			}	
		}
		pszInfix = getToken(pszInfix, szToken, MAX_TOKEN);
	}
	//once out of tokens, pops the stack till empty
	while (isEmpty(stack) == FALSE)
	{
		popped = pop(stack);
		//finds a left paren, means a right paren was missing
		if (popped.iCategory == CAT_LPAREN)
		{
			freeStack(stack);
			return WARN_MISSING_RPAREN;
		}
		addOut(out, popped);
	}
	freeStack(stack);
	//returns 0 if expression is normal
	return 0;
}
