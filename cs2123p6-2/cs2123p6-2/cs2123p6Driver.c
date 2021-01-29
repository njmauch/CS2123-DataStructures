/*******************************************************************************
cs2123p6Driver.c by Nathan Mauch
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
	MAXSTEPS szOrigin szDest
		Prints the maximum number of steps to get from szOrigin to szDest.
	PRTALTS szOrigin szDest
		For the specified origin and destination, it determines all possible paths (considering time)
		and prints them.
	DELETE szAirport
		Deletes the vertex and all flights involving this airport. It should free the edge
		nodes that are no longer needed. Flights involving this airport may also be on the
		other airport's adjacency lists and need to be deleted.
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
#include "cs2123p6.h"
 //Declaration of global structure
AltPath altPath;
extern AltPath altPath;

int main(int argc, char * argv[])
{
	altPath = newAltPath();
	Graph graph = newGraph();
	getFlights(graph);
	free(graph);
	free(altPath);

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
	char szDest[4];						//Store destination name
	char *pszRemainingTxt;              // Stores remaining text from getToken

	int iMax = 0;
	int iIndent = 0;        // Declare/initialize indent to 0 for PRTs
	int iPrevArrTm2400;     // Declared var for PRTCHRON function
	int iVertex = 0;        // Vertex value used for given airport

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
			//if flight to be inserted is in deleted airport, make airport active again
			if (graph->vertexM[findAirport(graph, flight.szOrigin)].bExists == FALSE || graph->vertexM[findAirport(graph, flight.szDest)].bExists == FALSE)
			{
				graph->vertexM[findAirport(graph, flight.szOrigin)].bExists = TRUE;
				graph->vertexM[findAirport(graph, flight.szDest)].bExists = TRUE;
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
			//checks if the airport exists or not
			if (graph->vertexM[iVertex].bExists == FALSE)
			{
				printf(" Warning: This Airport does not exist\n");
				continue;
			}
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
			//checks if the airport exists or not
			if (graph->vertexM[iVertex].bExists == FALSE)
			{
				printf(" Warning: This Airport does not exist\n");
				continue;
			}
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
			//checks if the airport exists or not
			if (graph->vertexM[iVertex].bExists == FALSE)
			{
				printf(" Warning: This Airport does not exist\n");
				continue;
			}
			iPrevArrTm2400 = 0;
			prtTraversalChron(graph, iVertex, iIndent, iPrevArrTm2400);
		}
		// Makes sure to update all bVisted flags to FALSE
		setNotVisited(graph);
		if (strcmp(szRecordType, "PRTALTS") == 0)
		{
			//checks if the airport exists or not
			sscanf(pszRemainingTxt, "%s %s", szAirport, szDest);
			if (graph->vertexM[findAirport(graph, szAirport)].bExists == FALSE || graph->vertexM[findAirport(graph, szDest)].bExists == FALSE)
			{
				printf(" Warning: This Airport does not exist\n");
				continue;
			}
			
			prtAlts(graph, findAirport(graph, szAirport), findAirport(graph, szDest));
		}
		// Makes sure to update all bVisted flags to FALSE
		setNotVisited(graph);
		/* If szRecordType is MAXSTEPS, scans the given origin airport
		** and destination airport and calls maxStepsChronInit
		** function to find the maximum number of flights it takes to
		** reach the destination airport
		*/
		if (strcmp(szRecordType, "MAXSTEPS") == 0)
		{
			//checks if the airport exists or not
			sscanf(pszRemainingTxt, "%s %s", szAirport, szDest);
			if (graph->vertexM[findAirport(graph, szAirport)].bExists == FALSE || graph->vertexM[findAirport(graph, szDest)].bExists == FALSE)
			{
				printf(" Warning: This Airport does not exist\n");
				continue;
			}
			
			iMax = maxStepsChronInit(graph, findAirport(graph, szAirport), findAirport(graph, szDest), iPrevArrTm2400);
			printf(" Maximum chain chron for %s to %s contains %d steps\n", szAirport, szDest, iMax);
		}
		// Makes sure to update all bVisted flags to FALSE
		setNotVisited(graph);
		/* If szRecordType is DELETE, scans the given airport
		** and calls function deleteAirport to set airport.bExists to false
		** and frees all of the edgeNodes associated to that airport
		*/
		if (strcmp(szRecordType, "DELETE") == 0)
		{
			sscanf(pszRemainingTxt, "%s", szAirport);
			if (graph->vertexM[findAirport(graph, szAirport)].bExists == FALSE)
			{
				printf(" Warning: This Airport does not exist\n");
				continue;
			}
			deleteAirport(graph, szAirport);
		}
	}
}

/****************************** insertFlight ***********************************
int insertFlight(Graph graph, Flight flight, EdgeNode **eList)
Purpose:
	Inserts the given flight into the graph and updates the edge connections.
Parameters:
	O Graph graph           Graph containing connections of airports/flights
	I Flight flight         Flight containing information used for insertion
	I EdgeNode **eList      Flights successor or predecessor list
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
	Searches through graphs EdgeNodes
Parameters:
	I EdgeNode *e			 Beginning of successor or predecessor list to be searched
							 through
	I char szFlightNr[3]     Flight name
	O EdgeNode **ePrecedes   Double pointer to return preceding edge node to be
							 inserted into graph
Notes:
	(UPDATE TO ADD ANY NEW NOTES)
Returns:
	NULL - Edge Connection not in graph
	e - Node of value to be inserted
*******************************************************************************/
EdgeNode *searchEdgeNode(EdgeNode *e, char szFlightNr[3], EdgeNode **ePrecedes)
{
	*ePrecedes = NULL;
	for (e; e != NULL; e = e->pNextEdge) {
		if (strcmp(szFlightNr, e->flight.szFlightNr) == 0) {
			printf("Duplicate flight number\n");
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
/******************************* newAltPath **************************************
AltPath newAltPath()
Purpose:
	Allocates memory and initializes a new global variable altPath used to store
	paths of flight plans from origin to destination
Parameters:
	N/A
Notes:
	1. If malloc fails, throws an error.
Returns:
	Returns a newly allocated global variable altPath
*******************************************************************************/
AltPath newAltPath()
{
	AltPath ap = (AltPath)malloc(sizeof(AltPathImp));
	if (ap == NULL)
		errExit("could not allcate AltPath");
	ap->iAltCnt = 0;
	return ap;
}
/******************************* newPath **************************************
Path newPath()
Purpose:
	Allocates memory and initializes a new path used for saving the path of a given
	origin airport to destination airport
Parameters:
	N/A
Notes:
	1. If malloc fails, throws an error.
Returns:
	Returns a newly allocated path.
*******************************************************************************/
Path newPath()
{
	Path *path = (Path *)malloc(sizeof(Path));
	if (path == NULL)
		errExit("count not allocate Path");
	path->iStepCnt = 0;
	return *path;
}
/******************************* prtAlts **************************************
void prtAlts(Graph graph, int iOriginVertex, int iDestVertex)
Purpose:
	Given an origin and destination vertex and prints out all of the alternative
	paths possible to reach the destination considering time
Parameters:
	I Graph graph       Graph containing connections of airports/flights.
	I int iOriginVertex Vertex value of the origin airport
	I int iDestVertex   Vertex value of the destination airport
Notes:
	1. Uses global structure altPath to print out all possible paths of origin
	to destination.
	2. Invokes determinePaths to determine the path of alternative paths
Returns:
	N/A
*******************************************************************************/
void prtAlts(Graph graph, int iOriginVertex, int iDestVertex)
{
	int flightDurHours, flightDurMins;
	int iCurStep = 0;
	int iPrevArrTime = 0;
	altPath->iAltCnt = 0;
	//creates empty path to be filled by determinePaths 
	Path path = newPath();
	int i, j;
	//function determinePaths to find all alternative paths
	determinePaths(graph, iOriginVertex, iDestVertex, iCurStep, path, iPrevArrTime);
	//if no alternative paths found print this warning
	if (altPath->iAltCnt == 0)
	{
		printf(" No paths for flights from %s to %s\n", graph->vertexM[iOriginVertex].szAirport, graph->vertexM[iDestVertex].szAirport);
		return;
	}
	//headers for printing out alternative paths
	printf(" Alternatives for flights from %s to %s\n", graph->vertexM[iOriginVertex].szAirport, graph->vertexM[iDestVertex].szAirport);
	printf(" Alt TDur             Org Dst Fl Dep - Arr Dura\n");
	//loop through global structure altPath to print out alternative paths
	for (i = 0; i < altPath->iAltCnt; i++)
	{
		//convert total duration of flights into seperate hours and minutes
		flightDurHours = altPath->altPathM[i].iTotalDuration / 100;
		flightDurMins = altPath->altPathM[i].iTotalDuration % 100;
		printf("%4d %d Hours %d mins\n", i + 1, flightDurHours, flightDurMins);
		//loop through the steps of individual path and print them to output
		for (j = 0; j < altPath->altPathM[i].iStepCnt; j++)
		{
			iPrevArrTime = calcArr2400(altPath->altPathM[i].stepM[j]->flight.iDepTm2400, altPath->altPathM[i].stepM[j]->flight.iDurationMins, altPath->altPathM[i].stepM[j]->flight.iZoneChange);
			printf("%25s %s %s %04d-%04d  %3d\n", altPath->altPathM[i].stepM[j]->flight.szOrigin, altPath->altPathM[i].stepM[j]->flight.szDest, altPath->altPathM[i].stepM[j]->flight.szFlightNr, altPath->altPathM[i].stepM[j]->flight.iDepTm2400, iPrevArrTime, altPath->altPathM[i].stepM[j]->flight.iDurationMins);
		}
	}
}
/******************************* determinePaths **************************************
void determinePaths(Graph graph, int iVertex, int iDestVertex, int iCurStep, Path path, int iPrevArrTm2400)
Purpose:
	Recursive function that is given origin and destination to determine path to reach destination
	airport. Given empty path that is then insterted into global altPath if path is found to
	destination airport
Parameters:
	I Graph graph        Graph containing connections of airports/flights.
	I int iVertex	     Vertex value of the current airport
	I int iDestVertex    Vertex value of the destination airport
	I int iCurStep       Subscript into path->stepM to save an edge pointer
	O Path Path			 Path that stores steps of flights to reach destination
	I int iPrevArrTm2400 Arrival time of the flight
Notes:
	1. Inserts path into global structure altPath
Returns:
	N/A
*******************************************************************************/
void determinePaths(Graph graph, int iVertex, int iDestVertex, int iCurStep, Path path, int iPrevArrTm2400)
{
	EdgeNode *e;

	//base case
	if (graph->vertexM[iVertex].bVisited == TRUE)
		return;
	//sets airport to visited
	graph->vertexM[iVertex].bVisited = TRUE;
	//if destination is reached inserts path into global structure altPath
	if (iVertex == iDestVertex)
	{
		path.iStepCnt = iCurStep;			//number of steps taken to reach destination
		//calculates the total time of the flight
		path.iTotalDuration = calcArr2400(path.stepM[iCurStep - 1]->flight.iDepTm2400, path.stepM[iCurStep - 1]->flight.iDurationMins, path.stepM[iCurStep - 1]->flight.iZoneChange) - path.stepM[0]->flight.iDepTm2400;
		altPath->altPathM[altPath->iAltCnt] = path;			//inserting the path of flights into altPath->altPathM
		altPath->iAltCnt += 1;
		return;
	}
	//loops through successer list to find out if possible to reach destination airport
	for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
	{

		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		//checks if flight can be reached in reasonable time
		if (iPrevArrTm2400 <= e->flight.iDepTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS && iVertex != e->iDestVertex)
		{
			path.stepM[iCurStep] = e;			//saves edge node into path.stepM
			determinePaths(graph, e->iDestVertex, iDestVertex, iCurStep + 1, path, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));
		}
		//sets airport back to false in case is a part of another alternative path
		graph->vertexM[e->iDestVertex].bVisited = FALSE;
	}
}
/****************************** maxStepsChronInit ******************************
int maxStepsChronInit(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400)
Purpose:
	Returns the max number of steps to main function to be printed to output
Parameters:
	I Graph graph        Graph containing connections of airports/flights
	I int iVertex        Vertex value of current airport
	I int iDestVertex	 Vertex of destination airport
	I int iPrevArrTm2400 Time of flights arrival
Notes:
	1. Invokes maxStepsChron to find the max number of steps
Returns
	Maximum number of steps to reach destination airport
*******************************************************************************/
int maxStepsChronInit(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400)
{
	int i;
	int iReturn;
	//sets all airports to not visited
	setNotVisited(graph);
	iReturn = maxStepsChron(graph, iVertex, iDestVertex, iPrevArrTm2400);
	if (iReturn == 0)
	{
		return 0;
	}
	//condition to fix amount of max steps to be correct
	else
	{
		return iReturn - 1;
	}
}