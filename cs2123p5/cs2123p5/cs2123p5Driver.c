/*******************************************************************************
cs2123p5Driver.c by Nathan Mauch
Purpose:
	Shows connections of major airports and their flights to one another.  Keeps
	track of all flights and their necessary information such as flight number,
	destination, duration of flight etc.
Command Parameters:
	N/A
Input:
	Input File:
		The standard input file stream contains flight information that is
		stored and updated to the adjacency lists.  Input stream also
		contains commands that result in a function being called and
		performing the necessary task.
	FLIGHT - Contains information relating to a flight and its values.
		szFlightNr   szOrigin   szDest   iDepTm2400   iDurationMIns  iZoneChange
		   %s            %s      %s         %d           %d              %d
	PRTFLIGHTBYORIGIN - For each airport, it prints the flights where it
		is the origin.
	PRTONE szAirport - Prints specified airport, flightNr/szOrigin for flights
		that have it as a destination, and prints flightNr-szDest of flights
		that have is an origin.
	PRTALL - Print all the airports as is done in PRTONE.
	PRTSUCC szOrigin - Print all the successors of the specified airport in a
		depth first manner with indentation.  If the szOrigin doesn't exist,
		show a warning.  For each flight, print the flightNr, destination,
		departure time, and arrival time. You must use the bVisited flag to
		avoid an infinite loop.  Unlike PRTCHRON, this does not consider times
		of the flights when examining successors.
	PRTCHRON szOrigin - This is similar to PRTSUCC; however, PRTSUCC prints all
		of the successors.  PRTCHRON only prints successors which have a time
		reasonably after the preceding flight.
Results:
	Creates an ordered adjacency list of airports each having their own
	predecessor and successor list of flights.  Prints the necessary information
	for each command that is called in an organized manner.
Returns:
	0 - normal
Notes:
	(UPDATE TO ADD ANY NOTES)
 ******************************************************************************/
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p5.h"

int main(int argc, char * argv[])
{
	Graph graph = newGraph();
	getFlights(graph);
}
/******************************* getFlights*************************************
void getFlights(Graph graph)
Purpose:
	Parses through Input File and determines what action should be done
	based on tokens/commands.  If FLIGHT token, updates graph to store a new
	airport and updates predecessors/successors.  Otherwise, If token is a
	variant of PRT, calls necessary function for it.
Parameters:
	O Graph graph       Graph used to track connections of flights/airports.
Notes:
	1. Retrieves a line feed, gets the first token, and determines what
	   action should be done by comparing token to functions.
Returns:
	N/A
*******************************************************************************/
void getFlights(Graph graph)
{
	Flight flight;      // Initializes new flight to store flight values

	char szInputBuffer[MAX_LINE_SIZE];  // Stores Input buffer for line feed
	char szRecordType[MAX_TOKEN];       // Stores token used for STRCMP           
	char szAirport[4];                  // Stores Airport Name
	char *pszRemainingTxt;              // Stores remaining text from getToken

	int iIndent = 0;        // Declare/initialize indent to 0 for PRTs
	int iPrevArrTm2400;     // Declared var for PRTCHRON function
	int iVertex = 0;        // Vertex value used for given airport
	int i;                  // For-loop iterator

	/* While the input file contains line to be parsed, retrieves line until EOF,
	** retrieves the first TOKEN from the line, and then determines what action
	** should be done with the given TOKEN.
	*/
	while (fgets(szInputBuffer, MAX_LINE_SIZE, stdin) != NULL) {
		// Checks if lines first character is a line-feed 
		if (szInputBuffer[0] == '\n')
		{
			continue;
		}
		// Prints scanned line
		printf("%s", szInputBuffer);
		/* getToken gets first TOKEN in line and stores it in szRecordType and
		** stores remaining line text in pszRemainingTxt.
		*/
		pszRemainingTxt = getToken(szInputBuffer, szRecordType, sizeof(szRecordType) - 1);
		/* If szRecordType is FLIGHT token, scans lines and stores values into
		** corresponding flight variables and updates graph values.
		*/
		if (strcmp(szRecordType, "FLIGHT") == 0) {
			sscanf(pszRemainingTxt, "%s %s %s %d %d %d",
				flight.szFlightNr,
				flight.szOrigin, flight.szDest,
				&flight.iDepTm2400,
				&flight.iDurationMins,
				&flight.iZoneChange);
			/* Checks to see if origin airport is already is graph.  If new
			** airport, inserts a new vertex for that origin airport.
			*/
			if (findAirport(graph, flight.szOrigin) < 0) {
				insertVertex(graph, flight.szOrigin);
			}
			/* Checks to see if destination airport is already is graph.  If new
			** airport, inserts a new vertex for that destination airport.
			*/
			if (findAirport(graph, flight.szDest) < 0) {
				insertVertex(graph, flight.szDest);
			}
			// inserts/updates the flights predecessor list
			insertFlight(graph, flight, &graph->vertexM[findAirport(graph, flight.szDest)].predecessorList);
			// inserts/updates the flights successors list
			insertFlight(graph, flight, &graph->vertexM[findAirport(graph, flight.szOrigin)].successorList);
		}
		/* If szRecordType is PRTFLIGHTBYORIGIN token, calls prtFlightsByOrigin
		** function.
		*/
		if (strcmp(szRecordType, "PRTFLIGHTBYORIGIN") == 0)
		{
				prtFlightsByOrigin(graph);
		}
		/* If szRecordType is PRTFLIGHTBYDEST token, calls PRTFLIGHTBYDEST
		** function.
		*/
		if (strcmp(szRecordType, "PRTFLIGHTBYDEST") == 0)
		{
				prtFlightsByDest(graph);
		}
		/* If szRecordType is PRTONE token, scans the given airport and calls
		** findAiport function to retrieve vertex from graph.  Then calls prtOne
		** function.
		*/
		if (strcmp(szRecordType, "PRTONE") == 0)
		{
				sscanf(pszRemainingTxt, "%s", szAirport);
				iVertex = findAirport(graph, szAirport);
				prtOne(graph, iVertex);
		}
		// If szRecordType is PRTALL token, calls printAll function.
		if (strcmp(szRecordType, "PRTALL") == 0)
		{
				prtAll(graph);
		}
		/* If szRecordType is PRTSUCC, scans the given airport and calls
		** findAiport function to retrieve vertex from graph.  Then calls
		** prtTraversal function.
		*/
		if (strcmp(szRecordType, "PRTSUCC") == 0)
		{
				sscanf(pszRemainingTxt, "%s", szAirport);
				iVertex = findAirport(graph, szAirport);
				prtTraversal(graph, iVertex, iIndent);
		}
		setNotVisited(graph);
		/* If szRecordType is PRTCHRON, scans the given airport and calls
		** findAiport function to retrieve vertex from graph.  Then calls
		** iPrevArrTm2400 function to calculate arrival and then calls
		** prtTraversalChron function.
		*/
		if (strcmp(szRecordType, "PRTCHRON") == 0)
		{
				sscanf(pszRemainingTxt, "%s", szAirport);
				iVertex = findAirport(graph, szAirport);
				iPrevArrTm2400 = 0;
				prtTraversalChron(graph, iVertex, iIndent, iPrevArrTm2400);
		}
		// Makes sure to update all bVisted flags to FALSE
		setNotVisited(graph); 
	}
}

/****************************** insertFlight ***********************************
int insertFlight(Graph graph, Flight flight, EdgeNode **eList)
Purpose:
	Inserts the given flight into the graph and updates the edge connections.
Parameters:
	O Graph graph           Graph containing connections of airports/flights
	I Flight flight         Flight containing information used for insertion
	  EdgeNode **eList      ????
Notes:
	(UPDATE TO ADD ANY NOTES)
Returns
	(Not too sure correct return value)
*******************************************************************************/
int insertFlight(Graph graph, Flight flight, EdgeNode **eList)
{
	EdgeNode *eNew;         // Stores newly allocate EdgeNode with values
	EdgeNode *eFind;        // EdgeNode used to search if connection exists
	EdgeNode *ePrecedes;    // Previous EdgeNode holder

	// Searches through edges to check if connection already exist
	eFind = searchEdgeNode(*eList, flight.szFlightNr, &ePrecedes);
	// If 
	if (eFind != NULL) {
		eNew = allocateEdgeNode(graph, flight);
		eNew->pNextEdge = eFind->pNextEdge;
		eFind->pNextEdge = eNew;
		return;
	}
	eNew = allocateEdgeNode(graph, flight);
	if (ePrecedes == NULL) {
		eNew->pNextEdge = *eList;
		*eList = eNew;
	}
	else {
		eNew->pNextEdge = ePrecedes->pNextEdge;
		ePrecedes->pNextEdge = eNew;
	}
	return;
}
/****************************** insertVertex ***********************************
void insertVertex(Graph graph, char szAirport[])
Purpose:
	Updates the number of vertices in the graph, gives the given airport
	a designated vertex value for the graph, and sets all its values.
Parameters:
	O Graph graph           Graph containing connections of airports/flights
	I char szAirport[]      Airport being stored into graph
Notes:
	1. Since new airport, sets all values for that vertex/airport to its proper
	starting values.
Returns
	N/A
*******************************************************************************/
void insertVertex(Graph graph, char szAirport[])
{
	// Insert name of aiport into graph
	strcpy(graph->vertexM[graph->iNumVertices].szAirport, szAirport);
	// Sets all elements in the new airport values to its necessary values.
	graph->vertexM[graph->iNumVertices].bExists = TRUE;
	graph->vertexM[graph->iNumVertices].bVisited = FALSE;
	graph->vertexM[graph->iNumVertices].successorList = NULL;
	graph->vertexM[graph->iNumVertices].predecessorList = NULL;
	graph->iNumVertices++;
}
/******************************* newGraph **************************************
Graph newGraph()
Purpose:
	Allocates memory and initializes a new graph used for the connections of
	airports and flights.
Parameters:
	N/A
Notes:
	1. If malloc fails, throws an error.
Returns:
	Returns a newly allocated graph.
*******************************************************************************/
Graph newGraph() {
	// Declare and allocate memory for a new graph
	Graph graph = malloc(sizeof(GraphImp));
	// Check if malloc correctly worked, if not throws an error
	if (graph == NULL)
		errExit("Unable to allocate memory");
	// New graph so no vertices. Set to 0.
	graph->iNumVertices = 0;
	// Return new graph
	return graph;
}

/************************** *allocateEdgeNode **********************************
EdgeNode *allocateEdgeNode(Graph graph, Flight flight)
Purpose:
	Allocates memory and initializes a new EdgeNode, sets its elements values
	based on the given flight, and return the new EdgeNode.
Parameters:
	I Graph graph       Graph containing connections of airports/flights.
	I Flight flight     Flight containing information for new EdgeNode
Notes:
	1. If malloc fails, throws an error.
	2. Implements findAiport function to retrieve origin/dest. vertex values.
Returns:
	Returns a newly allocated EdgeNode.
*******************************************************************************/
EdgeNode *allocateEdgeNode(Graph graph, Flight flight)
{
	// allocate memory for a new EdgeNode
	EdgeNode *eNew;
	eNew = (EdgeNode *)malloc(sizeof(EdgeNode));

	// check if memory is available
	if (eNew == NULL)
		errExit("Unable to allocate memory for EdgeNode");
	// Set EdgeNode values based on given flight
	eNew->flight = flight;
	eNew->iDestVertex = findAirport(graph, flight.szDest);
	eNew->iOriginVertex = findAirport(graph, flight.szOrigin);
	// make this node's next edge NULL
	eNew->pNextEdge = NULL;
	// return new EdgeNode
	return eNew;
}

/************************** *searchEdgeNode **********************************
EdgeNode *searchEdgeNode(EdgeNode *e, char szFlightNr[3], EdgeNode **ePrecedes)
Purpose:
	Searches through graphs EdgeNodes???
Parameters:
	? EdgeNode *e
	I char szFlightNr[3]     Flight name
	? EdgeNode **ePrecedes
Notes:
	(UPDATE TO ADD ANY NEW NOTES)
Returns:
	NULL - Edge Connection not in graph
	e - ?
*******************************************************************************/
EdgeNode *searchEdgeNode(EdgeNode *e, char szFlightNr[3], EdgeNode **ePrecedes)
{
	*ePrecedes = NULL;
	for (e; e != NULL; e = e->pNextEdge) {
		if (strcmp(szFlightNr, e->flight.szFlightNr) == 0) {
			errExit("Duplicate flight number");
			return e;
		}
		if (strcmp(szFlightNr, e->flight.szFlightNr) < 0) {
			return NULL;
		}
		*ePrecedes = e;
	}
	return NULL;
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
