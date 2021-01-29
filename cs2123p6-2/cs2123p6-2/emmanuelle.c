#include "cs2123p6.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/****************************** prtOne ******************************
void prtOne(Graph graph, int iVertex);
Purpose:
	Prints information for airport at specified vertex.

Parameters:
	I Graph graph           Graph containing connections of airports/flights.
	I int iVertex           Specified vertex that the function will print information
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

	int iCount = 0;     //counter for ellipsis in final print                                 
	int iCount2 = 0;    //counter for ellipsis in final print
	EdgeNode *e;        //edgenode traversal
	int i;	        //shortcut to refer to vertex directly
	//>>PRTONE SAT
	//VX APT PREDECESSORS                          SUCCESSORS
	// 0 SAT H1/IAH  ... ... ... ... ... ... ...   S1-IAH S2-MCO S3-ATL 

	//check to see if airport exists
	if (iVertex >= graph->iNumVertices)
	{
		//if not, print warning and then return
		printf("WARNING: airport at specified vertex does not exist");
		return;
	}
	//print demographics of airport at vertex
	printf("  %d %s ", iVertex, graph->vertexM[iVertex].szAirport);	//vertex #, airport name

	//get list of predecessors
	//to be initialized in for loop
	for (e = graph->vertexM[iVertex].predecessorList; e != NULL; e = e->pNextEdge)
	{
		//for each successor found, print it 
		printf("%s/%s ", e->flight.szFlightNr, e->flight.szOrigin);
		//and decrement iEllipsisCnt
		iCount++;
	}
	//print ellipsis if iEllipsisCnt > 0;
	if (iCount < 7)
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
	I Graph graph       Graph containing connections of airports/flights.
Notes:
	Prints all vertices in a list. Utilizes prtOne() to accomplish this.
Returns
	N/A
*******************************************************************************/
void prtAll(Graph graph)
{
	int iVertex;    //vertex # initialized for loop

	//for each vertex in a list, call printall
	for (iVertex = 0; iVertex < graph->iNumVertices; iVertex++)
		prtOne(graph, iVertex);
}

/****************************** prtTraversal ******************************
void prtTraversal(Graph graph, int iAirportVertex, int indent);
Purpose:
 Prints flights that begin at the specified origin.

Parameters:
	I Graph graph          	Graph containing connections of airports/flights.
	I int iAirportVertex        Specified vertex that the function will begin printing
					from.
	I int indent          	int representing indentions in printed output.
Notes:
	prtTraversal is a recursive function. It will continue to make calls to itself
	until the criteria for the base case is met (if aiport at specified vertex has
	already been visited).
Returns
	N/A
*******************************************************************************/
void prtTraversal(Graph graph, int iAirportVertex, int iIndent)
{

	EdgeNode *e;    //edgenode traversal for for-loop
	int i;          //indent for-loop iterator
	//base case
	if (graph->vertexM[iAirportVertex].bVisited == TRUE)
		return;

	//change bVisited to TRUE
	graph->vertexM[iAirportVertex].bVisited = TRUE;

	/* Run through the vertex succesorList and print its information out
	** until NULL.  Uses recursion to recall itself to print successors.
	*/
	for (e = graph->vertexM[iAirportVertex].successorList; e != NULL; e = e->pNextEdge)
	{
		// check if it has been visited
		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		else
		{
			//print indentions
			for (i = 0; i < iIndent; i++)
			{
				printf("    ");
			}
			//print information
			printf(" %s-%s %04d-%04d\n",
				e->flight.szFlightNr,
				e->flight.szDest,
				e->flight.iDepTm2400,
				calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));
		}
		//recall function
		prtTraversal(graph, e->iDestVertex, iIndent + 1);
		graph->vertexM[e->iDestVertex].bVisited = FALSE;
	}
}
/****************************** maxStepsChron ***********************************
int maxStepsChron(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400);
Purpose:
	Determines the maximum number of steps to go to from the origin to the destination.
Parameters:
	O Graph graph           Graph containing connections of airports/flights
	I int iVertex 			Starting vertex
	I int iDestVertex 		Destination vertex in which the function calculates max steps
	I iPrevArrTm2400 		//ADD NOTE HERE
Notes:
	N/A
Returns
	Returns the maximum number of steps to go from the origin to the destination.
*******************************************************************************/


// 	This is invoked due to the MAXSTEPS command.  It determines the maximum number of steps to go from the origin to the destination:
// •	Initially, iVertex is the origin vertex.  On subsequent calls, it is a successor vertex.
// •	This uses a depth first traversal to return the maximum number of steps to reach the destination.  
// •	Since we have many paths we must set and reset the bVisited flag in this function.  If we have already visited a destination in the current path, we won't include the flight in the count.  
// •	This only considers successor flights which occur after the arrival of the previous flight.  Please see Time Considerations in the Assignment 5 documentation. 

int maxStepsChron(Graph graph, int iVertex, int iDestVertex, int iPrevArrTm2400)
{
	EdgeNode *e;
	int iReturn = 0;

	//base cases
	if (graph->vertexM[iVertex].bVisited)
		return 0;
	if (iVertex == iDestVertex)
		return 1;

	//change bVisited to TRUE	
	graph->vertexM[iVertex].bVisited = TRUE;

	for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
	{
		int iMax = maxStepsChron(graph, e->iDestVertex, iDestVertex, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange));

		if (iPrevArrTm2400 <= e->flight.iDepTm2400 + SAFE_DELTA_BETWEEN_FLIGHTS && iVertex != e->iDestVertex)	//check to see if successor flight occurs after the
		{																												//arrival of the previous flight.
			if (iMax > iReturn)
				iReturn = iMax;
		}
	}
	//change bVisited to FALSE
	graph->vertexM[iVertex].bVisited = FALSE;

	if (iReturn != 0)
		iReturn++;

	return iReturn;
}
