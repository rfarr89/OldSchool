//============================================================================
// Name        : RLF_A3.cpp
// Author      : Ryan Farrell (UHID 1488274)
// Class       : COSC3360 Principles of Operating Systems, Spring 2017
// Instructor  : Jehan-Francois Paris
// Description : Bridge program
//	Simulates vehicles passing over a load-zoned bridge. The bridge's maximum
// allowed weight in tons is passed in the command line as an integer. A text file
// containing information on vehicles wishing to cross the bridge is loaded using 
// i/o redirection. Each line of the text file contains a vehicle ID as a string, 
// that vehicle's arrival delay (in seconds) after the preceeding vehicle as an 
// interger, that vehicle's weight in tons rounded up to the next integer value,
// and the seconds that vehicle will take to cross the bridge as an integer value.
// ...
// mutex bridgeLock is used to lock access to the bridgeLoad variable
// condition bridgeAccess is used to signal that the next vehicle may attempt to enter the bridge
// ...
// pthreads are created for each vehicle in the time they arrive
// ...
// vehicles attempt to cross in the order they arrive
// ...
// if a vehicle is too heavy for the bridge it will not be permitted to pass (output dictating such)
// and its thread will be terminated early
// ...
// The program terminates when all vehicles have crossed over the bridge (or have tried to
// and were turned away).
//
//============================================================================

#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <vector>
#include <unistd.h>

using namespace std;

static int maxWeight;		// maximum weight limit of the bridge in tons
static int bridgeLoad;		// current weight of vehicles on the bridge, in tons
static int countCrossed;	// keeps count of how many vehicles have crossed the bridge successfully
static int countFailed;		// keeps count of how many vehicles cannot cross the bridge due to their weight exceeding the maximum
static pthread_mutex_t bridgeLock = PTHREAD_MUTEX_INITIALIZER;	// initial mutex to control access to bridgeLoad (manipulated in enterBridge() and leaveBridge())
static pthread_cond_t bridgeAccess = PTHREAD_COND_INITIALIZER;	// condition variable that signals another vehicle may access the bridge

void *vehicle(void *);

// struct vehicleLine used in the VehicleTable
// represents a vehicle or a line of data in the VehicleTable
struct vehicleLine {
	string ID;
	int arrivalDelay;
	int weightLoad;
	int crossTime;
};

void enterBridge(int weight) {
	bridgeLoad = bridgeLoad + weight;		// add weight to bridgeLoad
}

void leaveBridge(int weight) {
	bridgeLoad = bridgeLoad - weight;		// remove weight from bridgeLoad
}

int main(int argc, char **argv) {
	/*
	 With ./a.out 10
		argc = 2
		argv[0] = "./a.out"
		argv[1] = "10"
 	 	Use atoi() to convert string "10" into int
	 */
	// 1) set bridge max weight from passed value
	maxWeight = atoi(argv[1]);
	bridgeLoad = 0;						// initial bridge load is 0 (bridge is assumed empty at start)
	// 2) read in vehicle data
	vehicleLine newLine;				// temporarily holds input data of incoming vehicle(s)
	string vehicleID;					// temporarily holds input vehicle ID of incoming vehicle(s)
	int arrival;						// temporarily holds input arrival time of incoming vehicle(s)
	int wload;							// temporarily holds input weight of incoming vehicle(s)
	int cTime;							// temporarily holds input crossing time of incoming vehicle(s)
	int nchildren = 0; 					// counts number of vehicles and thus child threads to create
	vector<vehicleLine> VehicleTable;	// vector of type vehicleLine to hold vehicle data
	while (cin >> vehicleID) {
		cin >> arrival;					// pull data into temporary holders
		cin >> wload;
		cin >> cTime;
		newLine.ID = vehicleID;
		newLine.arrivalDelay = arrival;
		newLine.weightLoad = wload;
		newLine.crossTime = cTime;
		VehicleTable.push_back(newLine);		// add newLine data to VehicleTable
		nchildren++;					// increment count of vehicles/child threads
	}
	// test print VehicleTable
/*
	cout << "\n=============================================" << endl;
	cout << "Vehicle Table:\n";
	for (int i = 0; i < nchildren; i++) {
		cout << VehicleTable[i].ID << "\t" << VehicleTable[i].arrivalDelay << "\t" << VehicleTable[i].weightLoad;
		cout << "\t" << VehicleTable[i].crossTime << "\n";
	}
	cout << "=============================================" << endl;
*/

	// test if maxWeight is valid
	// (would do this before reading in vehicle data but using i/o redirection)
	if (maxWeight < 0) {
		cout << "Invalid input detected for maximum weight." << endl;
		cout << "Terminating program." << endl;
		exit(1);
	}
	else if (maxWeight == 0) {
		string selector;
		cout << "You have input a maximum bridge capacity of 0 tons." << endl;
		while (true) {
			cout << "\nAre you certain you want a bridge with zero allowed weight? (Y/n): ";
			cin >> selector;
			if (selector == "Y" || selector == "y") {
				// continue
				break;
			}
			else if (selector == "N" || selector == "n") {
				cout << "Terminating program.\n";
				exit(0);
			}
		}
	}
	// else maxWeight is an acceptable weight and we may continue
	cout << "\n=============================================" << endl;
	cout << "Maximum bridge load is set to " << maxWeight << " tons." << endl;
	cout << "=============================================" << endl;

	// 3) set mutexes and condition variables
	// *** DID THIS ABOVE IN GLOBAL VARIABLES ***
	
	countCrossed = 0;		// initialize count of successful crossings to 0
	countFailed = 0;		// initialize count of failed crossings to 0
	// 4) run simulation (vehicles pass over the bridge in time order, assuming the bridge weight limit allows)
	// use vehicle data to create child_thread(s)
	int maxchildren = 64;					// maxchildren is maximum number of expected vehicles in the input file, set by Professor Paris
	pthread_t child[maxchildren];			// keeps track of all the pthread tid's
	int count = 0;							// count keeps track of index of current vehicle arriving at the bridge
	while (count < nchildren) {				// create pthread(s) in the time the vehicles arrive
		sleep(VehicleTable[count].arrivalDelay);									// sleep for arrivalDelay seconds
		pthread_create(&child[count], NULL, vehicle, (void*) &VehicleTable[count]);	// create pthread for this vehicle
		count++;																	// increment count to next vehicle
	}
	for (int i = 0; i < nchildren; i++) {
		pthread_join(child[i], NULL);			// join pthread(s) to wait for their completion
	}
	// end of program
	cout << "\n=============================================" << endl;
	cout << nchildren << " vehicle(s) attempted to cross the bridge." << endl;
	cout << countCrossed << " vehicle(s) successfully crossed over the bridge." << endl;
	cout << countFailed << " vehicle(s) were turned away for being too heavy." << endl;
	cout << "The current bridge load is " << bridgeLoad << " tons." << endl;
	cout << "=============================================" << endl;
	// end of program, shut things down...
	pthread_mutex_destroy(&bridgeLock);	// destroy mutex bridgeLock
	pthread_cond_destroy(&bridgeAccess);	// destroy condition variable bridgeAccess
	return 0;
} // main() = parent thread, creates and manages child thread(s)

void *vehicle(void *arg) {
	struct vehicleLine *thisVehicle = (struct vehicleLine*) arg;	// thisVehicle points to the address for VehicleTable[index of current vehicle]
	// print arrival message
	cout << "Vehicle " << thisVehicle->ID << " has arrived at the bridge" << endl;
	cout << "The current bridge load is " << bridgeLoad << " tons." << endl;

	// if vehicle weight exceeds bridge max weight terminate its thread
	if (thisVehicle->weightLoad > maxWeight) {
		// display error message explaining the vehicle is too heavy
		cout << "Vehicle " << thisVehicle->ID << " is too heavy to cross this bridge." << endl;
		cout << "It must turn around and find another route." << endl;
		countFailed++;						// increment count of failed crossings
		pthread_exit((void *) 1);				// exit thread with error
	}
	// else, continue
	pthread_mutex_lock(&bridgeLock);			// lock mutex (to change bridgeLoad)
		// check if vehicle can enter the bridge (wait for condition if bridgeLoad + current vehicle's weight exceeds maxWeight)
		while ((bridgeLoad + thisVehicle->weightLoad) > maxWeight) {
			pthread_cond_wait(&bridgeAccess, &bridgeLock);	// while we are weight-limited, wait for mutex 'bridgeLock' and signal 'bridgeAccess'
		}
		enterBridge(thisVehicle->weightLoad);			// update bridgeLoad
		// print entering bridge message
		cout << "Vehicle " << thisVehicle->ID << " is entering the bridge." << endl;
		cout << "The current bridge load is " << bridgeLoad << " tons." << endl;

		// if bridgeload < maxWeight (but not equal!), signal condition &bridgeAccess
		if (bridgeLoad < maxWeight) {
			pthread_cond_signal(&bridgeAccess);
		}
	pthread_mutex_unlock(&bridgeLock);				// unlock mutex (done updating bridgeLoad)
	sleep(thisVehicle->crossTime);					// sleep for crossTime seconds
	pthread_mutex_lock(&bridgeLock);				// lock mutex (to change bridgeLoad)
		leaveBridge(thisVehicle->weightLoad);			// update bridgeload
		// print bridge exit message
		cout << "Vehicle " << thisVehicle->ID << " has exited the bridge." << endl;
		cout << "The current bridge load is " << bridgeLoad << " tons." << endl;
	
		countCrossed++;						// increment count of successful crossings
		pthread_cond_signal(&bridgeAccess);			// signal the vehicle has left the bridge
	pthread_mutex_unlock(&bridgeLock);			// unlock the mutex

	//pthread_exit((void*) 0);						// terminate pthread in case it doesn't do so automatically
	return NULL;									// return NULL pointer, should terminate thread, if not un-comment above line
} // vehicle() = child threads, controls the actions of an individual vehicle attempting to cross the bridge
