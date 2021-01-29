#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "cs2123p6.h"
// Written By: Vedika Khanna 
/*******************************prtFlightsByDest********************************
void prtFlightsByDest(Graph graph)
Purpose:
	Print all flights from the graph in order by destination.
Parameters:
	O Graph graph       Graph used to track connections of flights/airports.
Notes:
	N/A
Returns:
	Print Statement with all flights information (Airport, Origin, Flight Number,
	Departure time, Arrival Time, and Duration of flights in order by Destination.
*******************************************************************************/
void prtFlightsByDest(Graph graph)
{
	EdgeNode *e;        //for-loop iterator for edges
	int iVertices;      //i
	//print header
	printf(" %-3s %3s %2s %3s %4s %4s \n", "Apt", "Orig", "Fl", "Dep", "Arr", "Dur");

	/* Go through all the vertices in the graph and print
	** out the necessary information for that flight.
	*/
	for (iVertices = 0; iVertices < graph->iNumVertices; iVertices++)
	{
		int bFirst = TRUE;
		// check if anything before flight
		if (graph->vertexM[iVertices].predecessorList == NULL)
		{
			continue;
		}
		// Run through the predecessor list and print its values
		for (e = graph->vertexM[iVertices].predecessorList; e != NULL; e = e->pNextEdge) {

			if (bFirst == TRUE)
			{
				printf(" %-3s %3s %3s %04d %04d %3d\n", graph->vertexM[iVertices].szAirport,
					e->flight.szOrigin,
					e->flight.szFlightNr,
					e->flight.iDepTm2400,
					calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange),
					e->flight.iDurationMins);
				bFirst = FALSE;
			}
			else
			{
				printf(" %7s %3s %04d %04d %3d\n", e->flight.szOrigin, e->flight.szFlightNr, e->flight.iDepTm2400,
					calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);
			}
		}
	}
}

/******************************************prtFlightsByOrigin**************************
void prtFlightsByOrigin(Graph graph)
Purpose:
	Print all flights from the graph in order by origin.
Parameters:
	O Graph graph       Graph used to track connections of flights/airports.
Notes:

Returns:
	Print Statement with all flights information (Airport, Flight Number, Destination,
	Departure time, Arrival Time, and Duration of flight in order by Origin.
*******************************************************************************/
void prtFlightsByOrigin(Graph graph)
{
	EdgeNode *e;        //for-loop iterator for edges
	int iVertices;      //current vertex value in the graph

	//print the header
	printf(" %-3s%3s %2s %3s  %3s  %3s\n", "Apt", "F#", "Dest", "Dep", "Arr", "Dur");
	// Go through the number of vertices in the list and print information
	for (iVertices = 0; iVertices < graph->iNumVertices; iVertices++)
	{
		int bFirst = TRUE;
		if (graph->vertexM[iVertices].successorList == NULL)
		{
			continue;
		}
		// Go through vertex successor list and prints its information
		for (e = graph->vertexM[iVertices].successorList; e != NULL; e = e->pNextEdge)
		{

			if (bFirst == TRUE)
			{
				printf(" %-3s%3s %3s  %04d %04d %3d\n", graph->vertexM[iVertices].szAirport,
					e->flight.szFlightNr,
					e->flight.szDest,
					e->flight.iDepTm2400,
					calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange),
					e->flight.iDurationMins);
				bFirst = FALSE;
			}
			else
			{
				printf("%7s %3s  %04d %04d %3d\n", e->flight.szFlightNr,
					e->flight.szDest,
					e->flight.iDepTm2400,
					calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange),
					e->flight.iDurationMins);
			}
		}
	}
}
