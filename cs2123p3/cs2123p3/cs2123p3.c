/*****************************************************************************
cs2123p3.c by Nathan Mauch
Purpose:
	This program runs a simulation of arrive and depart times of different customers
	and then displays the arrival and departures based on the time of each event.
Command Paramaters:
	n/a
Input:
	Standard input file with customers name, departure time, and event time.
	Sample data:
		Ant,Adam   10 5
		Mander,Sally 4 3
		King,May  6 6
Results:
	For each customer, prints the arrival and depart time.
Returns:
	0 - Normal
Notes:
	1. This program uses an ordered linked list to keep the events of arrival and 
	departure in order.
	2. Each customer has 2 events, arrival and departure.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cs2123p3.h"

//Main program

int main(int argc, char *argv[])
{
	// Create a simulation's object
	Simulation simulation = newSimulation();
	//calls to run simulation
	runSimulation(simulation);
	//free simulation and eventlist
	free(simulation->eventList);
	free(simulation);
	return 0;
}
/**************** removeLL *************************************
int removeLL(LinkedList list, Event *pValue)
Purpose:
	Removes node from the front of a linked list and returns the 
	Event structure. If list is empty it returns FALSE otherwise 
	returns TRUE.
Paramaters:
	I	LinkedList list		Linked list of customers and timing of events
							(arrival and departure)
	O	Event *pValue		Pointer to event structure which is modified
							and returned by this function
Returns:
	TRUE - List wasn't empty
	FALSE - List was empty
***************************************************************/
int removeLL(LinkedList list, Event  *pValue)
{
	NodeLL *pRemove;    // will point to node being removed
	// see if list is empty
	if (list->pHead == NULL)
		return FALSE;
	// return the element value via pValue
	*pValue = list->pHead->event;
	// set pRemove to point to the node being removed and remove it
	pRemove = list->pHead;
	list->pHead = list->pHead->pNext;
	free(pRemove);
	return TRUE;
}
/***************** insertOrderedLL ***************************
NodeLL *insertOrderedLL(LinkedList list, Event value)
Purpose:
	Inserts an event into an ordered linked list.
Paramaters:
	I	LinkedList list		Linked list of customers and timing of events
							(arrival and departure)
	I	Event value			Event structure of new customer to be inserted
Returns:
	pNew - pointer to new node
**************************************************************/
NodeLL *insertOrderedLL(LinkedList list, Event value)
{
	NodeLL *pNew, *pFind, *pPrecedes;
	//searches list if value already exist
	pFind = searchLL(list, value.iTime, &pPrecedes);
	if (pFind != NULL)
	{
		pNew = allocNodeLL(list, value);
		pNew->pNext = pFind->pNext;
		pFind->pNext = pNew;
		return pNew;
	}
	// Doesn't already exist.  Allocate a node and insert.
	pNew = allocNodeLL(list, value);
	// Check for inserting at the beginning of the list
	// this will also handle when the list is empty
	if (pPrecedes == NULL)
	{
		pNew->pNext = list->pHead;
		list->pHead = pNew;
	}
	else
	{
		pNew->pNext = pPrecedes->pNext;
		pPrecedes->pNext = pNew;
	}
	return pNew;
}
/******************************* searchLL *************************
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes)
Purpose:
	Searches for match in the list. if found returns pointer to node
	if not found returns null.
Paramaters:
	I	LinkedList list		List of customers to be searched if matching
							event time
	I	int match			Used to compare if event time to decide where to 
							insert node
	O	NodeLL **ppPrecedes	Double pointer to return preceding node value to be 
							inserted into ordered linked list
Returns:
	p - Node of value to be inserted
	NULL - if no match is found
******************************************************************/
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes)
{
	NodeLL *p;
	// NULL used when the list is empty 
	// or when we need to insert at the beginning
	*ppPrecedes = NULL;

	// Traverse through the list looking for where the key belongs or
	// the end of the list.
	for (p = list->pHead; p != NULL; p = p->pNext)
	{
		if (match == p->event.iTime)
			return p;
		if (match < p->event.iTime)
			return NULL;
		*ppPrecedes = p;
	}
	// Not found return NULL
	return NULL;
}
/********************* newLinkedList *********************************
Purpose:
	Creates new linked list and allocates memory for linked list.
Paramaters:
	n/a
Returns:
	list - New linked list with null value
*********************************************************************/
LinkedList newLinkedList()
{
	LinkedList list = (LinkedList)malloc(sizeof(LinkedListImp));
	// Mark the list as empty
	list->pHead = NULL;   // empty list
	return list;
}
/******************* allocNodeLL *************************************
Purpose:
	Allocates node for singly linked list
Paramaters:
	I	LinkedList list		list of customers and their even times
	I	Event Value			Event structure to be inserted into new node
Returns:
	errExit - If unable to allocate memory exits with error
	pNew - New node
*********************************************************************/
NodeLL *allocNodeLL(LinkedList list, Event value)
{
	NodeLL *pNew;
	//allocates memory for node
	pNew = (NodeLL *)malloc(sizeof(NodeLL));
	//checks if memory was allocated properly
	if(pNew == NULL)
		errExit("Unable to allocate memory");
	//inserts value in new node
	pNew->event = value;
	//sets next node to null
	pNew->pNext = NULL;
	return pNew;
}
/************************ newSimulation **************************************
Purpose:
	Creates a new simulation structure.
Paramaters:
	n/a
Returns:
	simulation - new simulation structure with null value
Notes:
	1. Sets iClock to 0
	2. Creates eventlist as a new linked list in the simulation structure
*****************************************************************************/
Simulation newSimulation()
{
	Simulation simulation = (Simulation)malloc(sizeof(SimulationImp));
	simulation->eventList = newLinkedList();
	simulation->iClock = 0;
	return simulation;
}
/*************************** runSimulation *************************************
Purpose:
	Runs through a simulation of arrival and departure events stored in a linked list.
	Displays the name and time of the event in the simulation to p3Out.txt
Paramaters:
	I	Simulation simulation		Structure containting linked list of events and
									times of arrivals and departures
Returns:
	n/a

********************************************************************************/
void runSimulation(Simulation simulation)
{
	Event event;
	//function call to read in p3Input to fill simulation structure
	readArriveAndDepart(simulation);
	//header of Time, Person, and Event
	printf("%6s %-12s %-10s\n", "Time", "Person", "Event");
	//moves through structure to display simulation
	while(removeLL(simulation->eventList, &event))
	{
		switch (event.iEventType)
		{
		//For arrivals print arrival time and person's name
		case EVT_ARRIVE:
			printf("%6d %-12s %-10s\n", event.iTime, event.person.szName, "Arrive");
			break;
		//For Departures, print depart time and person's name
		case EVT_DEPART:
			printf("%6d %-12s %-10s\n", event.iTime, event.person.szName, "Depart");
			break;
		default:
			errExit("Uknown event type: %d\n", event.iEventType);
			break;
		}
	}
	//end of simulation
	printf("%6d %-12s %-10s\n", event.iTime, "SIMULATION", "TERMINATES");
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
/******************** readArriveAndDepart **********************************************
Purpose:
	Reads standard input file p3Input.txt to gather customers name, departure time, and 
	duration of that event.
Paramaters:
	I	Simulation simulation		Structure that contains linked list to be filled with 
									person name, event times, event types
Returns:
	n/a
Notes:
	1. Scans in from stdin into szInputBuffer to be inserted into eventList linked list
	2. Inserts events into linked list in order by using function insertOrderedLL
	3. Keeps track of times of the event and the time the customer arrives
******************************************************************************************/
void readArriveAndDepart(Simulation simulation)
{
	Event event;


	char szInputBuffer[MAX_LINE_SIZE];
	int iEvent, iScanfCnt;
	//runs through p3Input.txt till null
	while (fgets(szInputBuffer,MAX_LINE_SIZE, stdin) != NULL)
	{
		//Counts number of items scanned to verify good data and then inserts into linked list
		iScanfCnt = sscanf_s(szInputBuffer, "%[^ ] %d %d", event.person.szName,(unsigned)_countof(event.person.szName), &event.person.iDepartTmUnits, &iEvent);
		if (iScanfCnt < 3)
			errExit("Bad customer data");
		//sets clock for arrival
		event.iTime = simulation->iClock;
		event.iEventType = EVT_ARRIVE;
		//inserts arrival event into linked list
		insertOrderedLL(simulation->eventList, event);
		event.iEventType = EVT_DEPART;
		//sets time for departure
		event.iTime = simulation->iClock + event.person.iDepartTmUnits;
		//inserts departure node into linked list
		insertOrderedLL(simulation->eventList, event);
		simulation->iClock += iEvent;
	}
}