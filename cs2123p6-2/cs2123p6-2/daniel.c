#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cs2123p6.h"
//Functions written by Daniel Tellez for PGM5   
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
	int i;  // For-loop iterator
	/* Traverses through graphs total vertices and compares the given airport
	** to the airports in the graph.  If match, returns vertex value.
	*/
	for (i = 0; i <= graph->iNumVertices; i++) {
		if (strcmp(szAirport, graph->vertexM[i].szAirport) == 0) {
			return i;
		}
	}
	// Airport not found, return -1.
	return -1;
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
void prtTraversalChron(Graph graph, int iVertex, int iIndent, int iPrevArrTm2400)
{
	EdgeNode *e;        // EdgeNode pointer used to traverse through edges
	int i;              // for-loop iterator for indentions
	// Checks if current airport has been visited 
	if (graph->vertexM[iVertex].bVisited == TRUE)
		return;
	// If new airport, set bVisited to TRUE
	graph->vertexM[iVertex].bVisited = TRUE;
	/* Run through the successor list for that aiport until NULL.
	** Check to see if successor flights have a reasonable time
	** to board.  If good, print flight and times.
	*/
	for (e = graph->vertexM[iVertex].successorList; e != NULL; e = e->pNextEdge)
	{
		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		// Check to see if the next flights departure time is reasonable
		if (e->flight.iDepTm2400 - iPrevArrTm2400 >= SAFE_DELTA_BETWEEN_FLIGHTS && iVertex != e->iDestVertex)
		{
			// print indentations 
			for (i = 0; i < iIndent; i++)
				printf("      ");
			// get the next flight arrival data
			int nextFlightArr = calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange);
			// Print flight information
			printf(" %s-%s %04d-%04d\n", e->flight.szFlightNr, e->flight.szDest, e->flight.iDepTm2400, nextFlightArr);
			// Get the next flights arrival and recursively call the function
			prtTraversalChron(graph, e->iDestVertex, iIndent + 1, nextFlightArr);
		}
		graph->vertexM[e->iDestVertex].bVisited = FALSE;
	}
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