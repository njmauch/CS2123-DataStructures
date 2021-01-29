#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cs2123p5.h"

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