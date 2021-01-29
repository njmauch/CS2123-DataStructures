/**********************************************************************
cs2123p4.h
Purpose:
	Defines constants:
		max constants
		error constants
		event type constants
		boolean constants
	Defines typedef for
		Widget
		Event (instead of Element)
		For Linked List
			NodeLL
			LinkedListImp
			LinkedList
		For Queues
			QElement
			NodeQ
			QueueImp
			Queue
		For the Servers
			ServerImp
			Server
		 For the Simulation
			SimulationImp
			Simulation
 Protypes
		Functions provided by student
		Other functions provided by Larry previously (program 2)
		Utility functions provided by Larry previously (program 2)
Notes:

**********************************************************************/

/*** constants ***/
// Maximum constants

#define MAX_TOKEN 50            // Maximum number of actual characters for a token
#define MAX_LINE_SIZE 100       // Maximum number of character per input line
#define MAX_ARRIVAL_TIME 600

// Error constants (program exit values)
#define ERR_EXIT            999  // Unexpected error
#define ERR_COMMAND_LINE    900  // invalid command line argument

// Event Constants
#define EVT_ARRIVAL          1     // when a widget arrives
#define EVT_SERVERM_COMPLETE 2     // when a widget completes with server M
#define EVT_SERVERW_COMPLETE 3     // when a widget completes with server W
#define EVT_SERVERX_COMPLETE 4     // when a widget completes with server X
#define EVT_SERVERY_COMPLETE 5     // when a widget completes with server Y
// exitUsage control 
#define USAGE_ONLY          0      // user only requested usage information
#define USAGE_ERR           -1     // usage error, show message and usage information

// boolean constants
#define FALSE 0
#define TRUE 1

/* EOF */
#define REACHED_EOF 1

/*** typedef ***/

// Widget typedef 
typedef struct
{
	long lWidgetNr;         // Identifies a widget
	int iStep1tu;           // Step 1 time units
	int iStep2tu;           // Step 2 time units
	int iArrivalTime;       // Arrival time in tu
	int iWhichServer;       // For the alternatives, this specifies which server
} Widget;

// Event typedef
typedef struct
{
	int iEventType;         // The type of event as an integer:
							//    EVT_ARRIVAL - arrival event
							//    EVT_SERVERM_COMPLETE - servicing by server M is complete
							//    EVT_SERVERW_COMPLETE - servicing by server W is complete
							//    EVT_SERVERX_COMPLETE - servicing by server X is complete
							//    EVT_SERVERY_COMPLETE - servicing by server Y is complete
	int iTime;              // The time the event will occur 
	Widget widget;          // The widget involved in the event.
} Event;

// typedefs for the Linked Lists used for the event list
typedef struct NodeLL
{
	Event event;
	struct NodeLL *pNext;
} NodeLL;

typedef struct
{
	NodeLL *pHead;
} LinkedListImp;

typedef LinkedListImp *LinkedList;

// typedefs for the queues

typedef struct
{
	Widget widget;
	int iEnterQTime;                // time widget was inserted in queue
} QElement;

typedef struct NodeQ
{
	QElement element;
	struct NodeQ *pNext;
} NodeQ;

typedef struct
{
	NodeQ *pHead;                   // points to first node in queue
	NodeQ *pFoot;                   // points to last node in queue
	long lQueueWaitSum;             // Sum of wait times for the queue
	long lQueueWidgetTotalCount;    // Total count of widgets that entered queue
	char szQName[12];
} QueueImp;

typedef QueueImp *Queue;

// typedefs for server
typedef struct
{
	char szServerName[12];
	int bBusy;                      // TRUE - server is busy, FALSE - server is free
	Widget widget;                  // Widget the server is currently working
} ServerImp;
typedef ServerImp *Server;

// typedefs for the Simulation
typedef struct
{
	int iClock;                     // clock time
	int bVerbose;                   // When TRUE, this causes printing of event information
	long lSystemTimeSum;            // Sum of times widgets are in the system
	long lWidgetCount;              // The number of widgets processed 
	char cRunType;                  // A - Alternative A, B - Alternative B, C - Current
	LinkedList eventList;
} SimulationImp;
typedef SimulationImp *Simulation;

/**********   prototypes ***********/

// linked list functions - you must provide the code for these (see course notes)
int removeLL(LinkedList list, Event  *pValue);
NodeLL *insertOrderedLL(LinkedList list, Event value);
NodeLL *searchLL(LinkedList list, int match, NodeLL **ppPrecedes);
LinkedList newLinkedList();
NodeLL *allocNodeLL(LinkedList list, Event value);

// queue functions
int removeQ(Queue queue, QElement *pFromQElement);
void  insertQ(Queue queue, QElement element);
NodeQ *allocNodeQ(Queue queue, QElement value);
Queue newQueue(char szQueueNm[]);

// simulation functions - you must provide code for this
void runSimulationA(Simulation simulation);
void runSimulationB(Simulation simulation);
void runSimulationC(Simulation simulation);
void generateArrival(Simulation simulation);

// functions in most programs, but require modifications
void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo);

// Utility routines provided by Larry (copy from program #2)
void errExit(char szFmt[], ...);

//Created functions for simulation
void arrival(Simulation simulation, Widget *pWidget);
void queueUp(Simulation simulation, Queue queue, Widget *pWidget);
void seize(Simulation simulation, Queue queue, Server server);
void leaveSystem(Simulation simulation, Widget *pWidget);
Server newServer(char szServerNm[]);
Simulation newSimulation();
