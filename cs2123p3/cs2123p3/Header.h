void runSimulation(Simulation sim, int iTimeLimit)
{
	char inputBuffer[100];
	char tempToken[15];
	int iEvent;
	Event event;
	sim->iClock = 0;
	while ((fgets(inputBuffer, 100, stdin) != NULL) && sim->iClock < iTimeLimit)
	{
		getToken(inputBuffer, event.person.szName, MAX_TOKEN);
		getToken(inputBuffer, tempToken, MAX_TOKEN);
		event.person.iDepartUnits = atoi(tempToken);
		event.iTime = sim->iClock;
		event.iEventType = EVT_ARRIVE;
		insertOrderedLL(sim->eventList, event);
		event.iEventType = EVT_DEPART;
		event.iTime = sim->iClock + event.person.iDepartUnits;
		insertOrderedLL(sim->eventList, event);
		getToken(inputBuffer, tempToken, MAX_TOKEN);
		iEvent = atoi(tempToken);
		sim->iClock += iEvent;
	}
	printf("%6s %-12s %-10s\n", "Time", "Person", "Event");
	NodeLL *p;
	for (p = sim->eventList->pHead; p != NULL; p = p->pNext)
	{
		if ((p->event.iEventType) == EVT_ARRIVE)
			printf("%6d %-12s %-10s\n", p->event.iTime, p->event.person.szName, "Arrive");
		if ((p->event.iEventType) == EVT_DEPART)
			printf("%6d %-12s %-10s\n", p->event.iTime, p->event.person.szName, "Depart");
	}
	printf("%6d %-12s %-10s\n", p->event.iTime, "SIMULATION", "TERMINATES");
}
//Method definition of newLinkedList
LinkedList newLinkedList()
{
	LinkedList list = (LinkedList)malloc(sizeof(LinkedListImp));
	list->pHead = NULL;
	return list;
}