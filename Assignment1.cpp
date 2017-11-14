//============================================================================
// Name			: Assignment1.cpp
// Author		: Ryan Farrell (UHID# 1488274)
// Course		: COSC3360
// Semester		: Spring 2017, (submitted 5/22/17)
// Instructor	: Jehan-François Pâris
// Version		: 1.0?
// Copyright	: Yes please
// Description	: A process scheduling simulator utilizing round-robin scheduling policy.
//
//		Simulates the execution of a stream of interactive processes by a time-shared system
//	with a very large memory, NCORES processor and one disk drive. Inputs are read in via
//	I/O redirection and are expected in the form of a list of elements resembling "REQUEST <int>"
//	where REQUEST corresponds to "NCORES", "SLICE", "NEW", "CORE", "DISK", "DISPLAY", or "INPUT"
//	and the integer provided is the parameter corresponding to the input request. Input data
//	is loaded into a vector of type DataTableLine (struct DataTableLine) named DataTable. The program
//	then produces a Process Table from DataTable. The Process Table, named ProcessTable, is of type
//	ProcessTableLine (struct ProcessTableLine). Process start times are pulled from ProcessTable
//	into an EventList which is a priority_queue of type EventListLine (struct EventListList).
//	EventList stays sorted by the nature of a priority_queue as the top event is processed and
//	popped off and new events are pushed onto the priority_queue. class Compare has been included to
//	specify that EventList is to be sorted by the EventTime parameter. a ReadyQueue and a DiskQueue are
//	implemented as deque's as a regular queue would make printing their contents difficult.
//		Requests are processed in the time order they are received (based on input time parameters).
//	Once a process terminates a Process Termination Report is generated and printed to the console.
//	This report includes a snapshot of the system at time of process termination including the number
//	of busy cores, disk busy/idle state, ReadyQueue and DiskQueue contents, and the contents of The
//	Process Table.
//		Program completes when the EventList no longer contains EventListItem's.
//
//	function:
//	1) main(): only function. Contains all code for creating the DataTable, ProcessTable, and EventList
//				along with the EventList processing routine.
//
//	struct DataTableLine:		outlines variables that are attached to each Data Table line
//	struct ProcessTableLine:	outlines variables that are attach to each Process Table Line
//	struct EventListLine:		outlines variables that are attach to each Event List Line
//
//	Class Compare():
//	bool Operator(EventListLine, EventListLine):
//						used to specify how to compare EventListLine objects in EventList in order to maintain
//						a sorted priority_queue
//
//
//============================================================================

#include <cstdlib>
#include <iostream>
#include <queue>
#include <deque>
#include <string>
#include <vector>

using namespace std;

// DataTableLine represents a line of information in the Data Table.
// DataTable will be implemented as a vector
struct DataTableLine {
	string Operation; 	// operation type
	int Parameter;		// time value in ms
};

// ProcessTableLine represents a line of information in the Process Table.
// ProcessTable will be implemented as a vector
struct ProcessTableLine {
	int PNum;		// process number
	int StartTime; 	// process start time
	int FLine;		// first line of process in DataTable
	int LLine;		// last line of process in DataTable
	int CLine;		// current line of process
	string PStatus;	// RUNNING, READY, or BLOCKED
};

// EventListLine represents a line of information in the Event List.
// This type is also used for nodes in the ReadyQueue and DiskQueue
// EventList will be implemented as a priority_queue
// ReadyQueue will be implemented as a deque
// DiskQueue will be implemented as a deque
struct EventListLine {
	int ProcNum;		// process number, corresponds to position in ProcessTable
	string EventType;	// holds the type of event, indicating device required (if any)
	int EventTime;		// holds the event time remaining, if exceeds slice the difference will be the event time the next time the process enters the event list
};

// class compare included to specify which parameter of EventListLine is compared to keep the priority_queue sorted
class Compare {
public:
	bool operator() (EventListLine lineA, EventListLine lineB) {
		if (lineA.EventTime >= lineB.EventTime)
			return true;
		else
			return false;
	}
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~MAIN~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main() {

	int clock = 0;				// clock represents current time in ms. Initiated to 0 as time will start at 0.
	int slice = 0;				// time slice initiated to 0, to be set by input file
	int ncores = 1;				// ncores keeps track of how many cores the system has. Default to 1, systems with 0 cores are useless.
	int DiskBusyTimes = 0;		// keeps track of disk busy time
	int DTSize = 0;				// keeps tracks of DataTableSize
	int PTSize = 0;				// keeps track of ProcessTableSize
	bool DiskFree = true;		// keeps track of whether or not the disk is free
	int diskRT = 0;				// keeps track of disk request time

	// build Data Table (input table)
	vector<DataTableLine> DataTable;		// create a vector for the Data Table
	string step;		// string used to hold data as it is pulled in from file
	int holder = 0;		// holds integers being fed into the program
	while(cin >> step) {
		if(step == "NEW" || step == "DISK" || step == "DISPLAY" || step == "INPUT") {
			// if step is recognized as a proper input then add it to the vector DataTable
			cin >> holder;		// pull the next value (assumed to be an integer) into int holder
			DataTableLine newDTL;
			newDTL.Operation = step;
			newDTL.Parameter = holder;
			DataTable.push_back(newDTL);
			DTSize++;	// increment DataTableSize
			continue;
		}
		else if (step == "NCORES") {
			cin >> holder;
			DataTableLine newDTL;
			newDTL.Operation = step;
			newDTL.Parameter = holder;
			DataTable.push_back(newDTL);
			DTSize++;
			ncores = holder;
			continue;
		}
		else if (step == "SLICE") {
			cin >> holder;
			DataTableLine newDTL;
			newDTL.Operation = step;
			newDTL.Parameter = holder;
			slice = holder;
			DataTable.push_back(newDTL);
			DTSize++;
			continue;
		}
		else if (step == "CORE"){	// break up CORE requests based on slice time
			cin >> holder;
			while (holder > slice){
				DataTableLine newDTL;
				newDTL.Operation = step;
				newDTL.Parameter = slice;
				holder = holder - slice;
				DataTable.push_back(newDTL);
				DTSize++;
			}
			DataTableLine newDTL;
			newDTL.Operation = step;
			newDTL.Parameter = holder;
			DataTable.push_back(newDTL);
			DTSize++;
			continue;
		}
		else{
			continue;	// Error! Input does not match list of steps. Do nothing with the data and move forward.
		}
	}

	// build process table from data table
	vector<ProcessTableLine> ProcessTable;	// create a vector for the Process Table
	int j = 0;
	for (int i = 0; i < DTSize; i++) {
		// pull data from DataTable into ProcessTable
		step = DataTable[i].Operation;
		// Add NEW processes to the ProcessTable. Also pull in data for NCORES and time SLICE.
		if (step == "NEW"){
			if (j > 0) {
				ProcessTable[j-1].LLine = i-1;						// if not the first process then set the past process's last line to the DataTable index prior
			}
			ProcessTableLine newLine;								// create new ProcessTableLine
			newLine.PNum = j;
			newLine.StartTime = DataTable[i].Parameter;				// add start time to the newLine from the DataTable
			newLine.FLine = i+1;									// add first line to the newLine, it is the line following "NEW" (i+1)
			newLine.CLine = i+1;									// set current line of process to the first line of the process in DataTable
			newLine.PStatus = "BLOCKED";							// set process status to BLOCKED until startTime is satisfied
			ProcessTable.push_back(newLine);						// add newLine to ProcessTable
			PTSize++;												// increment ProcessTableSize
			j++;													// increment j
		}
	}
	// set last line for final process in the process table to last line in the Data Table
	ProcessTable[(PTSize-1)].LLine = (DTSize-1);


/*	TEST PRINT TABLES
	// test print DataTable
	cout << "~~~~~~~~~~\n";
	cout << "DATA TABLE\n";
	cout << "~~~~~~~~~~\n";
	for (int i = 0; i < DTSize; i++){
		cout << DataTable[i].Operation << " " << DataTable[i].Parameter << "\n" ;
	}
	// test print ProcessTable
	cout << "\n\n\n~~~~~~~~~~~~~\n";
	cout << "PROCESS TABLE\n";
	cout << "~~~~~~~~~~~~~\n";
	for (int i = 0; i < PTSize; i++){
		cout << ProcessTable[i].PNum << " ";
		cout << ProcessTable[i].StartTime << " ";
		cout << ProcessTable[i].FLine << " ";
		cout << ProcessTable[i].CLine << " ";
		cout << ProcessTable[i].LLine << " ";
		cout << ProcessTable[i].PStatus << "\n";
	}
	cout << "\n\nClock: " << clock << "\n";
	cout << "NCORES: " << ncores << "\n";
	cout << "SLICE: " << slice << " MS\n";
*/


	// build event list from process table
	priority_queue<EventListLine, vector<EventListLine>, Compare> EventList;	// create a priority_queue for the Event List
	for (int i = 0; i < PTSize; i++) {					// add process start times to initialize EventList
		EventListLine newELL;							// create new EventListLine
		newELL.ProcNum = ProcessTable[i].PNum;			// set process number for new EventListLine
		newELL.EventTime = ProcessTable[i].StartTime;	// set event time to process start time
		newELL.EventType = "ARRIVAL";					// new processes are arriving
		EventList.push(newELL);							// add newELL to EventList
	}

	deque<EventListLine> ReadyQueue;						// create a deque for the CPU
	int RQSize = 0;											// keeps track of ready queue size
	deque<EventListLine> DiskQueue;							// create a deque for the disk
	int DQSize = 0; 										// keeps track of disk queue size
	int FreeCores = ncores;									// keeps track of how many cores are free, initialize to value of ncores
	int PTIndex = 0;										// keeps track of process table index for currEvent
	// simultaneous event actions and scheduling and printing
	// while event list is not empty:
	while (!EventList.empty()){
		//	get next event in EventList
		EventListLine currEvent;							// new EventListLine currEvent to hold info on next event
		currEvent.ProcNum = EventList.top().ProcNum;		// set currEvent to front of EventList
		for (int i = 0; i < PTSize; i++) {
			if (ProcessTable[i].PNum == currEvent.ProcNum) {
				PTIndex = i;								// set process table index for currEvent (may differ from process number)
				break;
			}
		}
		clock = EventList.top().EventTime;					// set currEvent to front of EventList
		//currEvent.EventTime = clock;
		currEvent.EventType = EventList.top().EventType;	// set currEvent to front of EventList
		EventList.pop();									// remove top event from event list
		if (currEvent.EventType == "ARRIVAL") {
			// process arrival
			EventListLine newEvent;
			newEvent.ProcNum = currEvent.ProcNum;												// set process#
			newEvent.EventTime = clock;															// set event time
			newEvent.EventType = DataTable[ProcessTable[PTIndex].FLine].Operation;	// set event type
			EventList.push(newEvent);	// push newEvent to front of EventList since eventtime = clock
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "CORE") {
			// core REQUEST event routine
			// check for free core
			if (FreeCores > 0) {
				EventListLine CoreCompAt;				// create new EventListLine for core completion event
				CoreCompAt.EventTime = clock + DataTable[ProcessTable[PTIndex].CLine].Parameter;	// event time corresponds to when the core will be free
				CoreCompAt.EventType = "CORECOMP";
				CoreCompAt.ProcNum = currEvent.ProcNum;
				ProcessTable[PTIndex].PStatus = "RUNNING";
				EventList.push(CoreCompAt);				// insert completion event to EventList
				FreeCores--;							// decrement FreeCores since a core is now in use
			}
			else {
				ProcessTable[PTIndex].PStatus = "READY";
				ReadyQueue.push_back(currEvent);		// enter process in ready queue
				RQSize++;
			}
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "CORECOMP") {
			// core COMPLETION routine
			// increment current line for current process in the process table
			ProcessTable[PTIndex].CLine++;
			// test if the process has completed, if so schedule completion event
			if (ProcessTable[PTIndex].CLine > ProcessTable[PTIndex].LLine) {
				// if current line of process exceeds last line of process, the process is complete
				EventListLine procComp;
				procComp.ProcNum = currEvent.ProcNum;
				procComp.EventTime = clock;
				procComp.EventType = "PROCCOMP";
				EventList.push(procComp);
			}
			// if process has not completed add next step to event list
			else {
				// add next event for process to event list
				EventListLine nextLine;
				nextLine.ProcNum = currEvent.ProcNum;
				nextLine.EventTime = clock;
				nextLine.EventType = DataTable[ProcessTable[PTIndex].CLine].Operation;
				EventList.push(nextLine);
			}
			// check ReadyQueue
			if (ReadyQueue.empty()) {
				FreeCores++;			// increment FreeCores (frees up a core)
			}
			// pull a process from the ready queue into a core
			else {
				EventListLine newProc;
				newProc.ProcNum = ReadyQueue.front().ProcNum;
				// set process table index for newProc
				for (int i = 0; i < PTSize; i++) {
					if (ProcessTable[i].PNum == newProc.ProcNum) {
						PTIndex = i;
						break;
					}
				}
				newProc.EventTime = clock + DataTable[ProcessTable[PTIndex].CLine].Parameter;
				newProc.EventType = "CORECOMP";
				EventList.push(newProc);				// push new core completion event to EventList
				ProcessTable[PTIndex].PStatus = "RUNNING";
				ReadyQueue.pop_front();						// pops event from ReadyQueue
				RQSize--;
				//FreeCores remains the same (1-1=0)
			}
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "DISK") {
			// disk REQUEST event routine
			// set disk request time to parameter from data table
			diskRT = DataTable[ProcessTable[PTIndex].CLine].Parameter;
			if (diskRT == 0) {
				// if disk request time is zero then proceed to next process request
				// this simulates zero-disk-delay from I/O buffer
				// schedule a disk completion event at this time, will process next
				EventListLine nextLine;
				nextLine.ProcNum = currEvent.ProcNum;
				nextLine.EventTime = clock;
				nextLine.EventType = "DISKCOMP";
				EventList.push(nextLine);
				// continue to disk completion event
				continue;
			}
			// if disk request time is >0 then test if disk is free
			else if (DiskFree) {
				EventListLine nextLine;
				nextLine.ProcNum = currEvent.ProcNum;
				nextLine.EventTime = clock + diskRT;
				nextLine.EventType = "DISKCOMP";
				EventList.push(nextLine);
			}
			// if disk isn't available then enter request in disk queue
			else {
				DiskQueue.push_back(currEvent);
				DQSize++;
			}
			// process becomes blocked whether in disk or queue
			ProcessTable[PTIndex].PStatus = "BLOCKED";
			continue;
		}
		else if (currEvent.EventType == "DISKCOMP") {
			// disk COMPLETION event routine
			// increment current line index for current process
			ProcessTable[PTIndex].CLine++;
			// if current line exceeds last line then schedule process completion event
			if (ProcessTable[PTIndex].CLine > ProcessTable[PTIndex].LLine) {
				// if current line of process exceeds last line of process, the process is complete
				EventListLine procComp;
				procComp.ProcNum = currEvent.ProcNum;
				procComp.EventTime = clock;
				procComp.EventType = "PROCCOMP";
				EventList.push(procComp);
			}
			else {
				// add next event for process to event list
				EventListLine nextLine;
				nextLine.ProcNum = currEvent.ProcNum;
				nextLine.EventTime = clock;
				nextLine.EventType = DataTable[ProcessTable[PTIndex].CLine].Operation;
				EventList.push(nextLine);
			}
			if (DiskQueue.empty()) {
				DiskFree = true;	// disk is marked free
			}
			else {
				// pull front request from diskqueue
				EventListLine newDisk;
				newDisk.ProcNum = DiskQueue.front().ProcNum;
				newDisk.EventTime = clock + DataTable[ProcessTable[newDisk.ProcNum].CLine].Parameter;
				newDisk.EventType = "DISKCOMP";
				DiskBusyTimes = DiskBusyTimes + newDisk.EventTime;	// add disk request time to DiskBusyTimes
				EventList.push(newDisk);
				DiskQueue.pop_front();					// pop front off diskqueue
				DQSize--;
			}
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "DISPLAY") {
			// user display routine
			ProcessTable[PTIndex].PStatus = "BLOCKED";
			// create new event list line with event time = clock + display request time
			EventListLine dispComp;
			dispComp.ProcNum = currEvent.ProcNum;
			dispComp.EventTime = clock + DataTable[ProcessTable[PTIndex].CLine].Parameter;
			dispComp.EventType = "DSPLCOMP";
			EventList.push(dispComp);
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "DSPLCOMP") {
			// user display complete routine
			// increment current line index for current process
			ProcessTable[PTIndex].CLine++;
			// if current line exceeds last line then schedule process completion event
			if (ProcessTable[PTIndex].CLine > ProcessTable[PTIndex].LLine) {
				// if current line of process exceeds last line of process, the process is complete
				EventListLine procComp;
				procComp.ProcNum = currEvent.ProcNum;
				procComp.EventTime = clock;
				procComp.EventType = "PROCCOMP";
				EventList.push(procComp);
			}
			else {
				// add next event for process to event list
				EventListLine nextLine;
				nextLine.ProcNum = currEvent.ProcNum;
				nextLine.EventTime = clock;
				nextLine.EventType = DataTable[ProcessTable[PTIndex].CLine].Operation;
				EventList.push(nextLine);
			}
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "INPUT") {
			// user input routine
			ProcessTable[PTIndex].PStatus = "BLOCKED";
			// create new event list line with event time = clock + input request time
			EventListLine inpComp;
			inpComp.ProcNum = currEvent.ProcNum;
			inpComp.EventTime = clock + DataTable[ProcessTable[PTIndex].CLine].Parameter;
			inpComp.EventType = "INPCOMP";
			EventList.push(inpComp);
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "INPCOMP") {
			// user input complete routine
			// increment current line index for current process
			ProcessTable[PTIndex].CLine++;
			// if current line exceeds last line then schedule process completion event
			if (ProcessTable[PTIndex].CLine > ProcessTable[PTIndex].LLine) {
				// if current line of process exceeds last line of process, the process is complete
				EventListLine procComp;
				procComp.ProcNum = currEvent.ProcNum;
				procComp.EventTime = clock;
				procComp.EventType = "PROCCOMP";
				EventList.push(procComp);
			}
			else {
				// add next event for process to event list
				EventListLine nextLine;
				nextLine.ProcNum = currEvent.ProcNum;
				nextLine.EventTime = clock;
				nextLine.EventType = DataTable[ProcessTable[PTIndex].CLine].Operation;
				EventList.push(nextLine);
			}
			// continue to next event
			continue;
		}
		else if (currEvent.EventType == "PROCCOMP") {
			// process completion routine
			ProcessTable[PTIndex].PStatus = "TERMINATED";
			// print report here
			cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
			cout << "Process Termination Report";
			cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
			cout << "Process " << currEvent.ProcNum << " terminates at t = " << clock << " ms.\n";
			cout << "Number of busy cores: " << (ncores - FreeCores) << "\n";
			cout << "Disk is ";
			if (DiskFree) {
				cout << "IDLE\n";
			}
			else {
				cout << "BUSY\n";
			}
			cout << "Ready queue: ";
			if (RQSize == 0) {
				cout << "empty\n";
			}
			else {
				// print ready queue contents
				for (int i = 0; i < RQSize; i++) {
					cout << "Process " << ReadyQueue[i].ProcNum << ", ";
				}
				cout << "\n";
			}
			cout << "Disk queue: ";
			if (DQSize == 0) {
				cout << "empty\n";
			}
			else {
				// print disk queue contents
				for (int i = 0; i < DQSize; i++) {
					cout << "Process " << DiskQueue[i].ProcNum << ", ";
				}
				cout << "\n";
			}
			for (int i = 0; i < PTSize; i++) {
				cout << "Process " << ProcessTable[i].PNum;
				cout << " started at " << ProcessTable[i].StartTime << " ms ";
				cout << "and is " << ProcessTable[i].PStatus << "\n";
			}
			cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
			ProcessTable.erase(ProcessTable.begin()+PTIndex);				// remove terminated process from process table
			PTSize--;									// decrement ProcessTableSize
		}
	}
	return 0;
};
