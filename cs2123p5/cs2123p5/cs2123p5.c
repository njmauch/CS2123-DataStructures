#include "cs2123p5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
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
}*/
void prtFlightsByOrigin(Graph graph)
{
	int i;
	EdgeNode *e;
	printf(" %-3s %2s %3s %4s %4s %4s \n", "Apt", "F#", "Dest", "Dep", "Arr", "Dur");
	for (i = 0; i < graph->iNumVertices; i++)
	{
		if (graph->vertexM[i].successorList == NULL)
		{
			continue;
		}
		int bFirst = TRUE;

		for (e = graph->vertexM[i].successorList; e != NULL; e = e->pNextEdge)
		{
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
void prtFlightsByDest(Graph graph)
{
	EdgeNode *e;
	int iVertices;


	printf("%3s %3s %3s %3s %3s %3s \n", "Apt", "Orig", "Fl", "Dep", "Arr", "Dur");



	for (iVertices = 0; iVertices < graph->iNumVertices; iVertices++)
	{
		if (graph->vertexM[iVertices].predecessorList != NULL)
		{
			int bFirst = TRUE;
			for (e = graph->vertexM[iVertices].predecessorList; e != NULL; e = e->pNextEdge)
			{
				if (bFirst)
				{
					printf("%-3s %3s %3s %3d %3d %3d \n", graph->vertexM[iVertices].szAirport, e->flight.szOrigin, e->flight.szFlightNr, e->flight.iDepTm2400, calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);
					bFirst = FALSE;
				}
				else
				{
					printf(" %6s %3s %3d %3d %3d \n", e->flight.szOrigin, e->flight.szFlightNr, e->flight.iDepTm2400,
						calcArr2400(e->flight.iDepTm2400, e->flight.iDurationMins, e->flight.iZoneChange), e->flight.iDurationMins);

				}
			}
		}
	}
}/*
 void prtFlightsByDest(Graph graph)
{
	int i;
	EdgeNode *e;
	printf(" %-3s %3s %2s %3s %4s %4s \n", "Apt", "Orig", "Fl", "Dep", "Arr", "Dur");
	for (i = 0; i < graph->iNumVertices; i++)
	{
		if (graph->vertexM[i].predecessorList == NULL)
		{
			continue;
		}
		int bFirst = TRUE;
		for (e = graph->vertexM[i].predecessorList; e != NULL; e = e->pNextEdge)
		{
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
}*/
/*
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
*/
void prtOne(Graph graph, int iVertex)
{

	int iCount = 0;
	int iCount2 = 0;                                    //counter for ellipsis in final print
	EdgeNode *e;
	int i;												//shortcut to refer to vertex directly
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
void prtAll(Graph graph)
{
	printf(" All Vertices In a List\n");
	//print header	
	printf(" Vx Apt Predecessors %53s\n", "Successors");
	int iVertex;							//vertex # initialized for loop

	//for each vertex in a list, call printall
	for (iVertex = 0; iVertex < graph->iNumVertices; iVertex++)
		prtOne(graph, iVertex);


}
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
		if (graph->vertexM[e->iDestVertex].bVisited == TRUE)
		{
			continue;
		}
		else
		{
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