#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cs2123p4.h"

int main(int argc, char *argv[])
{
	Simulation simulation = newSimulation();
	processCommandSwitches(argc, argv, simulation);
	generateArrival(simulation);
	runSimulationB(simulation);
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
	if (pNew == NULL)
		errExit("Unable to allocate memory", value);
	//inserts value in new node
	pNew->event = value;
	//sets next node to null
	pNew->pNext = NULL;
	return pNew;
}
int removeQ(Queue queue, QElement *pFromQElement)
{
	NodeQ *q;
	if (queue->pHead == NULL)
		return FALSE;
	q = queue->pHead;
	*pFromQElement = q->element;
	queue->pHead = q->pNext;
	if (queue->pHead == NULL)
		queue->pFoot = NULL;
	free(q);
	return TRUE;
}
void insertQ(Queue queue, QElement element)
{
	NodeQ *pPrecedes;
	NodeQ *pNew = allocNodeQ(queue, element);

	if (queue->pFoot == NULL)
	{   // insert at front of queue
		queue->pFoot = pNew;
		queue->pHead = pNew;
	}
	else
	{   // inserting after a node
		queue->pFoot->pNext = pNew;
		queue->pFoot = pNew;
	}
}
NodeQ *allocNodeQ(Queue queue, QElement value)
{
	NodeQ *pNew;
	pNew = (NodeQ *)malloc(sizeof(NodeQ));
	if (pNew == NULL)
	{
		errExit("Unable to allocate memory", value);
	}
	pNew->element = value;
	pNew->pNext = NULL;

	return pNew;
}
Queue newQueue(char szQueueNm[])
{
	Queue queue = (Queue)malloc(sizeof(QueueImp));
	queue->pHead = NULL;
	queue->pFoot = NULL;
	strcpy(queue->szQName, szQueueNm);
	queue->lQueueWaitSum = 0;
	queue->lQueueWidgetTotalCount = 0;
	return queue;
}
void runSimulationA(Simulation simulation)
{
	Queue queueW = newQueue("w");
	Server serverW = newServer("w");
	Event event;
	generateArrival(simulation);
	while (removeLL(simulation, &event))
	{
		simulation->iClock = event.iTime;  // advance clock
		switch (event.iEventType)
		{
		case EVT_ARRIVAL:
			arrival(simulation, &event.widget);
			queueUp(simulation, queueW, &event.widget);
			seize(simulation, queueW, serverW);
			break;
		case EVT_SERVERW_COMPLETE:
			release(simulation, queueW, serverW, &event.widget);
			leaveSystem(simulation, &event.widget);
			break;
		default:
			errExit("Unknown event type: %d\n", event.iEventType);
		}
	}
}
void runSimulationB(Simulation simulation)
{
	Queue queueW = newQueue("w");
	Server serverW = newServer("w");
	Queue queueM = newQueue("m");
	Server serverM = newServer("m");
	Event event;
	generateArrival(simulation);
	while (removeLL(simulation, &event))
	{
		simulation->iClock = event.iTime;  // advance clock
		switch (event.iEventType)
		{
		case EVT_ARRIVAL:
			arrival(simulation, &event.widget);
			queueUp(simulation, queueW, &event.widget);
			seize(simulation, queueW, serverW);
			break;
		case EVT_SERVERW_COMPLETE:
			release(simulation, queueW, serverW, &event.widget);
			leaveSystem(simulation, &event.widget);
			break;
		case EVT_SERVERM_COMPLETE:
			release(simulation, queueM, serverM, &event.widget);
			leaveSystem(simulation, &event.widget);
			break;
		default:
			errExit("Unknown event type: %d\n", event.iEventType);
		}
	}
}
void runSimulationC(Simulation simulation)
{
	Queue queueW = newQueue("w");
	Server serverW = newServer("w");
	Queue queueX = newQueue("x");
	Server serverX = newServer("x");
	Queue queueY = newQueue("y");
	Server serverY = newServer("y");
	Event event;
	generateArrival(simulation);
	while (removeLL(simulation, &event))
	{
		simulation->iClock = event.iTime;  // advance clock
		switch (event.iEventType)
		{
		case EVT_ARRIVAL:
			arrival(simulation, &event.widget);
			queueUp(simulation, queueW, &event.widget);
			seize(simulation, queueW, serverW);
			break;
		case EVT_SERVERW_COMPLETE:
			release(simulation, queueW, serverW, &event.widget);
			leaveSystem(simulation, &event.widget);
			break;
		case EVT_SERVERX_COMPLETE:
			release(simulation, queueX, serverX, &event.widget);
			leaveSystem(simulation, &event.widget);
			break;
		case EVT_SERVERY_COMPLETE:
			release(simulation, queueY, serverY, &event.widget);
			leaveSystem(simulation, &event.widget);
			break;
		default:
			errExit("Unknown event type: %d\n", event.iEventType);
		}
	}
}
void generateArrival(Simulation simulation)
{
	Event eventArrival;
	char szInputBuffer[MAX_LINE_SIZE];
	int iArrivalDelta;

	//set event types
	eventArrival.iEventType = EVT_ARRIVAL;

	//iterate with each \n delimited line from standard input
	//create an arrival event in our linked list with each iteration
	while (fgets(szInputBuffer, 100, stdin) != NULL)
	{
		if (szInputBuffer[0] == '\n')
			break;

		//scan data into eventArrival and iArrivalDelta
		sscanf(szInputBuffer, "%ld %d %d %d %d", &eventArrival.widget.lWidgetNr, &eventArrival.widget.iStep1tu, &eventArrival.widget.iStep2tu, &iArrivalDelta, &eventArrival.widget.iWhichServer);

		//populate the rest of eventArrival
		eventArrival.iTime = simulation->iClock;
		eventArrival.widget.iArrivalTime = simulation->iClock;

		//insert the arrival into our linked list
		insertOrderedLL(simulation->eventList, eventArrival);

		//advance the clock so that the next arrival time is correct
		simulation->iClock = simulation->iClock + iArrivalDelta;
	}
}
/******************** exitUsage *****************************
    void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    In general, this routine optionally prints error messages and diagnostics.
    It also prints usage information.

    If this is an argument error (iArg >= 0), it prints a formatted message 
    showing which argument was in error, the specified message, and
    supplemental diagnostic information.  It also shows the usage. It exits 
    with ERR_COMMAND_LINE.

    If this is a usage error (but not specific to the argument), it prints 
    the specific message and its supplemental diagnostic information.  It 
    also shows the usage and exist with ERR_COMMAND_LINE. 

    If this is just asking for usage (iArg will be -1), the usage is shown.
    It exits with USAGE_ONLY.
Parameters:
    I int iArg                      command argument subscript or control:
                                    > 0 - command argument subscript
                                    0 - USAGE_ONLY - show usage only
                                    -1 - USAGE_ERR - show message and usage
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
{
	switch (iArg)
	{
	case USAGE_ERR:
		fprintf(stderr, "Error: %s %s\n"
			, pszMessage
			, pszDiagnosticInfo);
		break;
	case USAGE_ONLY:
		break;
	default:
		fprintf(stderr, "Error: bad argument #%d.  %s %s\n"
			, iArg
			, pszMessage
			, pszDiagnosticInfo);
	}
	// print the usage information for any type of command line error
	fprintf(stderr, "p2 -c customerFileName -q queryFileName\n");
	if (iArg == USAGE_ONLY)
		exit(USAGE_ONLY);
	else
		exit(ERR_COMMAND_LINE);
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
void arrival(Simulation simulation, Widget *pWidget)
{
	simulation->iClock = pWidget->iArrivalTime;
	if (simulation->bVerbose == TRUE)
		printf("%d\t %ld\t Arrived\n", simulation->iClock, pWidget->lWidgetNr);
}
void queueUp(Simulation simulation, Queue queue, Widget *pWidget)
{
	QElement qElement;
	qElement.widget = *pWidget;
	qElement.iEnterQTime = simulation->iClock;

	insertQ(queue, qElement);

	queue->lQueueWidgetTotalCount++;

	if (simulation->bVerbose == TRUE)
		printf("%d\t %ld\t Enter %s\n", simulation->iClock, qElement.widget.lWidgetNr, queue->szQName);
}
void seize(Simulation simulation, Queue queue, Server server)
{
	// Only execute the body of the function(seize the server)
		//if the server is not marked busy
		if (server->bBusy == FALSE)
		{
			QElement qElement;
			Event eventServerComplete;
			int iWaited;

			//mark the server as busy, and remove a widget from the queue
			server->bBusy = TRUE;
			removeQ(queue, &qElement);

			//assign the widget to the server
			server->widget = qElement.widget;

			//update statistics
			iWaited = simulation->iClock - qElement.iEnterQTime;
			queue->lQueueWaitSum += iWaited;

			if (simulation->bVerbose == TRUE)
				printf("%d\t %ld\t Seized server %s\n", simulation->iClock, qElement.widget.lWidgetNr, server->szServerName);

			//set the values of our completion event
			eventServerComplete.iTime = simulation->iClock + server->widget.iStep1tu + server->widget.iStep2tu;
			eventServerComplete.widget.lWidgetNr = server->widget.lWidgetNr;
			eventServerComplete.widget.iWhichServer = server->widget.iWhichServer;
			eventServerComplete.widget.iArrivalTime = server->widget.iArrivalTime;

			if (eventServerComplete.widget.iWhichServer == 1)
			{
				eventServerComplete.iEventType = EVT_SERVERM_COMPLETE;

				if (simulation->bVerbose == TRUE)
					printf("%d\t %ld\t Leave Queue M, waited %d\n", simulation->iClock, qElement.widget.lWidgetNr, iWaited);
			}
			else
			{
				eventServerComplete.iEventType = EVT_SERVERW_COMPLETE;

				if (simulation->bVerbose == TRUE)
					printf("%d\t %ld\t Leave Queue W, waited %d\n", simulation->iClock, qElement.widget.lWidgetNr, iWaited);
			}

			//finally, store the event in our linked-list
			insertOrderedLL(simulation->eventList, eventServerComplete);
		}
}
void leaveSystem(Simulation simulation, Widget *pWidget)
{
	simulation->lWidgetCount++;

	//total widget time is when is when it arrived subtracted from the
	//the current clock time
	int iSpentInSystem = simulation->iClock - pWidget->iArrivalTime;
	simulation->lSystemTimeSum += iSpentInSystem;

	if (simulation->bVerbose == TRUE)
		printf("%d\t %ld\t Exit System, in system %d\n", simulation->iClock, pWidget->lWidgetNr, iSpentInSystem);
}
Server newServer(char szServerNm[])
{
	Server server = (Server)malloc(sizeof(ServerImp));
	strcpy(server->szServerName, szServerNm, MAX_LINE_SIZE);
	server->bBusy = FALSE;
	return server;
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
	simulation->lWidgetCount = 0;
	simulation->lSystemTimeSum = 0;
	return simulation;
}