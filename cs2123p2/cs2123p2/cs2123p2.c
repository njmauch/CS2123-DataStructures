/******************cs2123p2.c**************************************
cs2123p2.c by Nathan Mauch
Purpose:
	This program converts infix equations to postfix and evaluates them
	based on the operator in the query provided from cs2123p2Driver.c. 
Command paramaters:
	cs2123p2Driver.c cs2123p2.h
*******************************************************************/
#include "cs2123p2.h"
/******************convertToPostfix********************************
int convertToPostfix(char *pszInfix, Out out)
Purpose:
	Converts an expression from infix notation into
	postfix notation.
Paramaters:
	I	char *pszInfix	Expression in infix notation to get tokens from
	O	Out out			Stores the postfix expression
Notes:
	1. This program uses an array to gather tokens to put into a stack and then
	an Out array to store the postfix notation expression.
Return value:
	WARN_MISSING_RPAREN		The infix expression is missing a right parenthesis
	WARN_MISSING_LPAREN		The infix expression is missing a left parenthesis
	0						The expression is normal
*****************************************************************/
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
/************************printCustomerData**********************
   void printCustomerData(Customer customerM[], int iNumCustomer)
   Purpose:
		Prints out the the customers info, id and name as well as
		the sub struct with trait values as well.
   Parameters:
   I	Customer customerM[]	Array of customers and traits
   I	int iNumCustomer		Number of customers in array
   Notes:

   Return Value:
		None
		Example Output:
   ID         Customer Name
				   Trait      Value
   11111      BOB WIRE
				   GENDER     M
				   EXERCISE   BIKE
				   EXERCISE   HIKE
				   SMOKING    N
   22222      MELBA TOAST
				   GENDER     F
				   BOOK       COOKING
   33333      CRYSTAL BALL
				   SMOKING    N
				   GENDER     F
				   EXERCISE   JOG
				   EXERCISE   YOGA
**************************************************************/
void printCustomerData(Customer customerM[], int iNumCustomer)
{
	int i;
	int j;
	// Print a heading for the list of customers and traits
	printf("%-6s %-20s\n", "ID", "Customer Name");
	printf("%10s %-10s %-12s\n", " ", "Trait", "Value");
	for (i = 0; i < iNumCustomer; i++)
	{
		// Print the customer information
		printf("%-6s %-20s\n", customerM[i].szCustomerId, customerM[i].szCustomerName);

		// Print each of the traits
		for (j = 0; j < customerM[i].iNumberOfTraits; j++)
		{
			// Print a trait
			printf("%10s %-10s %-12s\n", " ", customerM[i].traitM[j].szTraitType, customerM[i].traitM[j].szTraitValue);

		}
	}
}
/*************************evaluatePostfix************************************************
void evaluatePostfix(Out out, Customer customerM[], int iNumCustomer, QueryResult resultM[]
Purpose:
	Evaluates the postfix equation after p2Query has been converted returning customers
	with traits that are true for the query.
Paramaters:
	O	Out out						Stores the postfix expression
	I	Customer customerM[]		Array of customer and traits
	I	int iNumCustomer			Number of customers in array
	O	QueryResult resultM[]		array (which corresponds to customerM bia subscript)
									of booleans showing which customers satisfied a query
Notes:
	Uses converted p2Query results. Global variables are referenced.
Return Value:
	N/A
****************************************************************************************/
void evaluatePostfix(Out out, Customer customerM[], int iNumCustomer, QueryResult resultM[])
{
	//Variable declaration
	int i, j;
	Element postElem;
	Stack stack = newStack();
	Element elemRight, elemLeft, elemResult, evalElem;
	Trait trait;

	for (i = 0; i < iNumCustomer; i++)
	{
		for (j = 0; j < out->iOutCount; j++)
		{
			//gets element from out array
			postElem = out->outM[j];

			//switch to decide what to do with element base on category
			switch (postElem.iCategory)
			{
			//For operands push to stack
			case(CAT_OPERAND):
				push(stack, postElem);
				break;
			//if operator get top 2 elements and then perform task based on operator
			case CAT_OPERATOR:
				//gets top two items from stack to be evaluated
				elemRight = pop(stack);
				elemLeft = pop(stack);
				strcpy(trait.szTraitValue, elemRight.szToken);
				strcpy(trait.szTraitType, elemLeft.szToken);
				//if statements to decide query result pending on operator
				if (strcmp(postElem.szToken, "AND") == 0)
				{
					elemResult.bInclude = elemLeft.bInclude && elemRight.bInclude;
					push(stack, elemResult);
				}
				if (strcmp(postElem.szToken, "OR") == 0)
				{
					elemResult.bInclude = elemLeft.bInclude || elemRight.bInclude;
					push(stack, elemResult);
				}
				if (strcmp(postElem.szToken, "NEVER") == 0)
				{
					elemResult.bInclude = never(&customerM[i], &trait);
					push(stack, elemResult);
				}
				if (strcmp(postElem.szToken, "ONLY") == 0)
				{
					elemResult.bInclude = only(&customerM[i], &trait);
					push(stack, elemResult);
				}
				if (strcmp(postElem.szToken, "=") == 0)
				{
					elemResult.bInclude = atLeastOne(&customerM[i], &trait);
					push(stack, elemResult);
				}
			}
		}
		while (!isEmpty(stack))
		{
			evalElem = pop(stack);
			if (evalElem.bInclude == FALSE)
			{
				resultM[i] = FALSE;
				break;
			}
			else
			{
				resultM[i] = TRUE;
			}
		}
	}
	freeStack(stack);
}
/*******************************atLeastOne*****************************************
int atLeastOne(Customer *pCustomer, Trait *pTrait)
Purpose:
	Determines if customer has certain trait.  If customer does have trait returns true
	if customer does not returns false.
Paramaters:
	I	Customer pCustomer		Customer structure which containts customer traits
	I	Trait	 pTrait			Trait of the customer we want customer to have
Notes:
	
Return Values:
	TRUE - Customer did have the trait
	FALSE - Customer did not have the trait
**********************************************************************************/
int atLeastOne(Customer *pCustomer, Trait *pTrait)
{
	int i;
	if (pCustomer == '\0')
		errExit("received a NULL pointer");

	for (i = 0; i < (pCustomer->iNumberOfTraits); i++)
	{
		//checks customer has at least one
		if (strcmp(pCustomer->traitM[i].szTraitType, pTrait->szTraitType) == 0 
			&& strcmp(pCustomer->traitM[i].szTraitValue, pTrait->szTraitValue) == 0)
			return TRUE;
	}
	return FALSE;
}
/*****************************only***********************************************
int only(Customer *pCustomer, Trait *pTrait)
Purpose:
	Determines whether customer has only a certain trait base on the type. If checking
	for types of books customer can only have one type of book type.
Paramaters:
	I	Customer pCustomer		Customer structure that containts traits as well.
	I	Trait	 pTrait			Trait of customer that we want only one of
*******************************************************************************/
int only(Customer *pCustomer, Trait *pTrait)
{
	int i;
	if (pCustomer == '\0')
		errExit("received a NULL pointer");
	// Look at each of the traits for a customer
	for (i = 0; i < (pCustomer->iNumberOfTraits); i++)
	{
		//checks if customer only has one type of certain trait
		if (strcmp(pCustomer->traitM[i].szTraitType, pTrait->szTraitType) == 0 
			&& strcmp(pCustomer->traitM[i].szTraitValue, pTrait->szTraitValue) != 0)
			return FALSE;
	}
	return TRUE;
}
