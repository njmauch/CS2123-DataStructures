#include "cs2123p6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/****************************** prtTraversalChron ******************************
void prtTraversalChron(Graph graph, int iVertex, int iIndent, int iPrevArrTm2400)
Purpose:
	Recursively depth-first traverses graph and prints flights that begin at
	the specified origin.  Indents and pritns successor flights and checks to
	see if successors flight time is >= to preceding flight's completion time +
	SAVE_DELTA_BETWEEN_FLIGHTS.
Parameters:
	I Graph graph           Graph containing list of flights and the connections
	I int iVertex           starting origin of flight
	I int iIndent           indentation value use for print spaces
	I int iPrevArrTm2400    Past flights arrival time
Notes:
	1. Only prints flight successors if their time is greater than the arrival
	   time of the current flight.
Returns:
	N/A
*******************************************************************************/
void prtTraversalChron(Graph graph, int iAirportVertex, int iIndent, int iPrevArrTm2400)
{
	EdgeNode *e;
	int i;
	//base case
	if (graph->vertexM[iAirportVertex].bVisited == TRUE)
		return;

	//change bVisited to TRUE
	graph->vertexM[iAirportVertex].bVisited = TRUE;
	for (e = graph->vertexM[iAirportVertex].successorList; e != NULL; e = e->pNextEdge)
	{

		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		if (iPrevArrTm2400 <= e->flight.iDepTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS && iAirportVertex != e->iDestVertex)
		{
			for (i = 0; i < iIndent; i++)
			{
				printf("      ");
			}
			printf(" %s-%s %04d-%04d\n", e->flight.szFlightNr, e->flight.szDest, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));
			prtTraversalChron(graph, e->iDestVertex, iIndent + 1, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));
		}
		graph->vertexM[e->iDestVertex].bVisited = FALSE;
	}
}
/****************************** prtFlightsByOrigin ******************************
void prtFlightsByOrigin(Graph graph)
Purpose:
 Prints all of the flights based on origin

Parameters:
	I Graph graph          		Graph containing connections of airports/flights.
Notes:
	N/A
Returns
	N/A
*******************************************************************************/
void prtFlightsByOrigin(Graph graph)
{
	int i;
	EdgeNode *e;
	//header 
	printf(" %-3s %2s %3s %4s %4s %4s \n", "Apt", "F#", "Dest", "Dep", "Arr", "Dur");
	//loops through all vertices in graph
	for (i = 0; i < graph->iNumVertices; i++)
	{
		//checks if any flights in successorlist or if the airport exist
		if (graph->vertexM[i].successorList == NULL || graph->vertexM[i].bExists == FALSE)
		{
			continue;
		}
		//boolean used to determine if first time printing this airport
		int bFirst = TRUE;
		for (e = graph->vertexM[i].successorList; e != NULL; e = e->pNextEdge)
		{
			//if first line in printing airport, prints airport name
			if (bFirst == TRUE)
			{
				printf(" %-3s %2s %3s   %04d %04d %3d\n", graph->vertexM[i].szAirport, e->flight.szFlightNr, e->flight.szDest, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);
				bFirst = FALSE;
			}
			else
			{
				printf(" %6s %3s   %04d %04d %3d\n", e->flight.szFlightNr, e->flight.szDest, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);
			}
		}
	}
}
/****************************** prtFlightsByDest ******************************
void prtFlightsByDest(Graph graph)
Purpose:
 Prints all of the flights based on destination

Parameters:
	I Graph graph          		Graph containing connections of airports/flights.
Notes:
	N/A
Returns
	N/A
*******************************************************************************/
 void prtFlightsByDest(Graph graph)
{
	int i;
	EdgeNode *e;
	//header
	printf(" %-3s %3s %2s %3s %4s %4s \n", "Apt", "Orig", "Fl", "Dep", "Arr", "Dur");
	//loops through each vertex to print 
	for (i = 0; i < graph->iNumVertices; i++)
	{
		//checks if predecessor list has any flights and if the airport exists
		if (graph->vertexM[i].predecessorList == NULL || graph->vertexM[i].bExists == FALSE)
		{
			continue;
		}
		//boolean used to determine if first time printing this airport
		int bFirst = TRUE;
		for (e = graph->vertexM[i].predecessorList; e != NULL; e = e->pNextEdge)
		{
			//if first line printed for airport print airport name
			if (bFirst == TRUE)
			{
				printf(" %-3s %3s %3s %04d %04d %3d\n", graph->vertexM[i].szAirport, e->flight.szOrigin, e->flight.szFlightNr, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);
				bFirst = FALSE;
			}
			else
			{
				printf(" %7s %3s %04d %04d %3d\n", e->flight.szOrigin, e->flight.szFlightNr, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);
			}
		}
	}
}
 /****************************** calcArr2400 ******************************
int calcArr2400(int iDepTime2400, int iDurationMins, int iZoneChange);
Purpose:
 Calculates the arrival time for a flight from one airport to another.
Parameters:
	I int iDepTime2400          departure time for the flight
	I int iDurationMins         length of flight time
	I int iZoneChange           int representing number of time zone changes
Notes:
	(UPDATE TO ADD ANY NEW NOTES)
Returns
	int - iArrivalTime (Arrival time for the flight)
*******************************************************************************/
int calcArr2400(int iDepTime2400, int iDurationMins, int iZoneChange)
{
	int iHours;     // holds number of hours of iDepTime2400
	int iMin;       // holds number of minutes of iDepTime2400
	int iZoneAdd;   // holds number of minutes of time zone changes
	int iTotalMin;  // holds total number of minutes of flight
	int iArr2400;      // holds conversions of iTotalMin to 2400 time style

	// obtain number of hours
	iHours = iDepTime2400 / 100;
	// obtain number of minutes
	iMin = iDepTime2400 % 100;
	// obtain number of minutes for time zone changes
	iZoneAdd = iZoneChange * 60;
	// find total number of minutes of flight
	iTotalMin = (iHours * 60) + iMin + iZoneAdd + iDurationMins;
	// convert total number of minutes to 2400 time
	iArr2400 = (((iTotalMin / 60) * 100) + (iTotalMin % 60));
	// return the new 2400 arrival time
	return iArr2400;
}
/****************************** prtOne ******************************
void prtOne(Graph graph, int iVertex);
Purpose:
 Prints information for airport at specified vertex.

Parameters:
	I Graph graph          		Graph containing connections of airports/flights.
	I int iVertex        		Specified vertex that the function will print information
								from.
Notes:
	Prints information including Vertex#, Airport, Predecessors and Successors.
	A maximum of 8 predecessors is assumed. If there are less than 8, empty spots
	are replaced with "....."
Returns
	N/A
*******************************************************************************/
void prtOne(Graph graph, int iVertex)
{

	int iCount = 0;
	int iCount2 = 0;                               
	EdgeNode *e;
	int i;												

	//check to see if airport exists
	if (iVertex >= graph->iNumVertices)
	{
		//if not, print warning and then return
		printf("WARNING: airport at specified vertex does not exist");
		return;
	}
	//print demographics of airport at vertex
	printf("  %d %s ", iVertex, graph->vertexM[iVertex].szAirport);	

	//get list of predecessors
    //to be initialized in for loop
	for (e = graph->vertexM[iVertex].predecessorList; e != NULL; e = e->pNextEdge)
	{
		//for each successor found, print it 
		printf("%s/%s ", e->flight.szFlightNr, e->flight.szOrigin);
		//increments count to determine number of "...... " to print
		iCount++;
	}
	//print "...... " for spacing purposes
	if (iCount <= 7)
	{
		for (i = iCount; i <= 7; i++)
		{
			printf("...... ");
		}
	}
	//get list of successors
	for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
	{
		//for each successor found, print it 
		printf("%s-%s ", e->flight.szFlightNr, e->flight.szDest);
		iCount2++;
	}
	//if no successors prints "...... "
	if (iCount2 == 0)
	{
		printf("...... ");
	}
	printf("\n");

}
/****************************** prtAll ******************************
void prtAll(Graph graph);
Purpose:
 Prints all vertices in a list/graph.

Parameters:
	I Graph graph          		Graph containing connections of airports/flights.
Notes:
	Prints all vertices in a list. Utilizes prtOne() to accomplish this.
Returns
	N/A
*******************************************************************************/
void prtAll(Graph graph)
{
	printf(" All Vertices In a List\n");
	//print header	
	printf(" Vx Apt Predecessors %53s\n", "Successors");
	int iVertex;							//vertex # initialized for loop

	//for each vertex in a list, call printall
	for (iVertex = 0; iVertex < graph->iNumVertices; iVertex++)
	{
		//if airport doesn't exist skip that airport
		if (graph->vertexM[iVertex].bExists == FALSE)
		{
			continue;
		}
		prtOne(graph, iVertex);
	}

}
/****************************** prtTraversal ******************************
void prtTraversal(Graph graph, int iAirportVertex, int indent);
Purpose:
 Prints flights that begin at the specified origin.

Parameters:
	I Graph graph          		Graph containing connections of airports/flights.
	I int iAirportVertex        Specified vertex that the function will begin printing
								from.
	I int indent          		int representing indentions in printed output.
Notes:
	prtTraversal is a recursive function. It will continue to make calls to itself
	until the criteria for the base case is met (if aiport at specified vertex has
	already been visited).
Returns
	N/A
*******************************************************************************/
void prtTraversal(Graph graph, int iAirportVertex, int iIndent)
{

	EdgeNode *e;
	int i;
	//base case
	if (graph->vertexM[iAirportVertex].bVisited == TRUE)
		return;

	//change bVisited to TRUE
	graph->vertexM[iAirportVertex].bVisited = TRUE;


	for (e = graph->vertexM[iAirportVertex].successorList; e != NULL; e = e->pNextEdge)
	{
		//if airport is visited continue to next edge
		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		else
		{
			//prints all successor flights including successor flights from each airport reach with indenting when getting to next airport
			for (i = 0; i < iIndent; i++)
			{
				printf("    ");
			}
			printf(" %s-%s %04d-%04d\n", e->flight.szFlightNr, e->flight.szDest, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));
		}
		prtTraversal(graph, e->iDestVertex, iIndent + 1);
		graph->vertexM[e->iDestVertex].bVisited = FALSE;
	}
}
/****************************** findAirport ************************************
int findAirport(Graph graph, char szAirport[]);
Purpose:
	Iterates through the graphs list of airports/vertices and finds the given
	airport and returns its vertex value.
Parameters:
	I Graph graph              Graph containing list of airports
	I char szAirport[]         Airports name wanted to be found
Notes:
	1. If airport is not found in the graph, returns -1.
Returns
	i = Aiport vertex
	-1 = Airport not found
*******************************************************************************/
int findAirport(Graph graph, char szAirport[])
{
	int i;
	for (i = 0; i <= graph->iNumVertices; i++)
	{
		//if airport is found return vertex of that airport
		if (strcmp(szAirport, graph->vertexM[i].szAirport) == 0)
		{
			return i;
		}
	}
	//if not found return -1
	return -1;
}
/****************************** setNotVisited ******************************
void setNotVisited(Graph graph)
Purpose:
	Sets all airports in graph bVisited flag to FALSE.
Parameters:
	O Graph graph       Graph containing connections of airports/flights
Notes:
	N/A
Returns
	N/A
*******************************************************************************/
void setNotVisited(Graph graph)
{
	int i;  // For-loop iterator    
	// Goes through list of vertices in graph and sets bVisted to FALSE
	for (i = 0; i < graph->iNumVertices; i++)
		graph->vertexM[i].bVisited = FALSE;
}
/****************************** deleteAirport ******************************
void deleteAirport(Graph graph, char szAirport[])
Purpose:
	Sets airport.bExists to FALSE to mark it deleted and frees each each node
	related to that airport, and flights that had it as origin or destination.
Parameters:
	I Graph graph       Graph containing connections of airports/flights\
	I char szAirport    Airport that is being deleted from graph
Notes:
	Invokes freeEdgeNode to free memory of each flight assoicated to airport
Returns
	N/A
*******************************************************************************/
void deleteAirport(Graph graph, char szAirport[])
{
	{
		int i;
		int iVertex = findAirport(graph, szAirport);
		EdgeNode *ePrecedes2 = NULL;
		EdgeNode *e;
		EdgeNode *ePrecedes = NULL;

		//Remove the iVertex from the successorList
		for (i = 0; i < graph->iNumVertices; i++)//loop through the graph
		{
			for (e = graph->vertexM[i].successorList; e != NULL; e = e->pNextEdge)//loop through successorList
			{
				if (e == NULL)//if the successorList is empty
					break;
				if (e->iDestVertex == iVertex)//Vertex is in the successorList
				{
					if (ePrecedes == NULL)//iVertex is the first edgeNode
					{
						if (e->pNextEdge == NULL)//Removing would make list empty
						{
							e = NULL;
						}
						else//make p next edge the start of the successorList
						{
							e = e->pNextEdge;
						}
					}
					else//iVertex is in the middle or the end of the list
					{
						ePrecedes->pNextEdge = e->pNextEdge;
					}

				}
				ePrecedes = e;
			}
			for (e = graph->vertexM[i].predecessorList; e != NULL; e = e->pNextEdge)//loop through successorList
			{
				if (e == NULL)//if the prereqList is empty
					break;
				if (e->iOriginVertex == iVertex)//Vertex is in the successorList
				{
					if (ePrecedes2 == NULL)//iVertex is the first edgeNode
					{
						if (e->pNextEdge == NULL)//Removing would make list empty
						{
							e = NULL;
						}
						else//make p next edge the start of the prereqList
						{
							e = e->pNextEdge;
						}
					}
					else//iVertex is in the middle or the end of the list
					{
						ePrecedes2->pNextEdge = e->pNextEdge;
					}

				}
				ePrecedes2 = e;
			}
			if (i == iVertex)//Remove the vertex from the graph
			{
				graph->vertexM[i].bExists = FALSE;
				freeEdgeNodes(graph, iVertex);
			}
		}
	}

}
/****************************** freeEdgeNodes ******************************
void freeEdgeNodes(Graph graph, int iVertex)
Purpose:
	Frees edge nodes (flights) from deleted airport
Parameters:
	I Graph graph       Graph containing connections of airports/flights\
	I int iVertex       Vertex value of airport
Notes:
	N/A
Returns
	N/A
*******************************************************************************/
void freeEdgeNodes(Graph graph, int iVertex)
{
	EdgeNode *e;
	EdgeNode *ePrecedes = NULL;
	EdgeNode *eRemove;
	//loops through successor list to free edge node
	for (e = graph->vertexM[iVertex].successorList; e != NULL; )
	{
		if (graph->vertexM[iVertex].bExists == FALSE)
		{
			if (ePrecedes == NULL)
			{
				graph->vertexM[iVertex].successorList = e->pNextEdge;
			}
			else
			{
				ePrecedes->pNextEdge = e->pNextEdge;
			}
			eRemove = e;
			if (e->pNextEdge == NULL)
			{
				graph->vertexM[iVertex].successorList = ePrecedes;
			}
			//moves to next edge
			e = e->pNextEdge;
			free(eRemove);
		}
		else
		{
			ePrecedes = e;
			e = e->pNextEdge;
		}
	}
	//loops through predecessor list to free edge nodes
	for (e = graph->vertexM[iVertex].predecessorList; e != NULL; )
	{
		if (graph->vertexM[iVertex].bExists == FALSE)
		{
			if (ePrecedes == NULL)
			{
				graph->vertexM[iVertex].predecessorList = e->pNextEdge;
			}
			else
			{
				ePrecedes->pNextEdge = e->pNextEdge;
			}
			eRemove = e;
			if (e->pNextEdge == NULL)
			{
				graph->vertexM[iVertex].predecessorList = ePrecedes;
			}
			e = e->pNextEdge;
			free(eRemove);
		}
		else
		{
			ePrecedes = e;
			e = e->pNextEdge;
		}
	}
}
/****************************** maxStepsChron ******************************
int maxStepsChron(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400)
Purpose:
	Finds the maximum number of steps to reach destination airport using depth first
	traversal. 
Parameters:
	I Graph graph        Graph containing connections of airports/flights
	I int iVertex        Vertex value of current airport
	I int iDestVertex	 Vertex of destination airport
	I int iPrevArrTm2400 Time of flights arrival
Notes:
	1. Only considers successor flights'
	2. Recursive function
Returns
	Maximum number of steps to reach destination airport
*******************************************************************************/
int maxStepsChron(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400)
{
	EdgeNode *e;
	int iReturn = 0;
	//base case
	if (graph->vertexM[iVertex].bVisited == TRUE)
		return 0;
	
	//if destination airport is reach
	if (iVertex == iDestVertex)
	{
		return 1;
	}
	graph->vertexM[iVertex].bVisited = TRUE;
	//loops through successor list to find steps of flights to reach destination
	for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
	{
		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		//checks time if reasonable to reach flight
		if (iPrevArrTm2400 <= e->flight.iDepTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS && iVertex != e->iDestVertex)
		{
			int iMax = maxStepsChron(graph, e->iDestVertex, iDestVertex, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));
			//sets iReturn to iMax if less than iMax
			if (iMax > iReturn)
			{
				iReturn = iMax;
			}
		}
	}
	graph->vertexM[iVertex].bVisited = FALSE;
	//increments iReturn if not 0
	if (iReturn != 0)
	{
		iReturn++;
	}
	return iReturn;
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