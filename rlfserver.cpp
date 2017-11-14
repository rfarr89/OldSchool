//============================================================================
// Name        : rlfserver.cpp
// Author      : Ryan Farrell
// Version     :
// Copyright   : Your copyright notice
// Description : Server program for Assignment 2
//		***Contains code provided in Explanations2.pdf and
//		BSD_Sockets_A_Quick_And_Dirty_Primer.pdf***
//
//
//============================================================================

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string.h>

//page 3: code that uses establish() and get_connection() to allow multiple connections to be dealt with
/* obligatory includes */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>

//#define PORTNUM 51189 						/* random port number, my birthday */

using namespace std;

int establish(unsigned short);
int get_connection(int);
int read_data(int, char *, int);
int write_data(int, char *, int);
void closeServer(int, int);

// struct TableLine for building the KeyTable. Each TableLine represents a line in KeyTable.
struct TableLine {
	string uname;		// uname is for usernames
	string key;			// key is for keys
};

int main() {
	string keyfile;					// holds input file name for user input
	string dataholder;				// holds data for file input
	int tablesize = 0;				// number of lines in the KeyTable
	unsigned short PORTNUM = 51189;	// PORTNUM is port number for server (chosen upon initialization, default to random number)
	int s;							// socket for server?
	int t;							// socket for calls?
	int inBufSize = 50;				// set incoming buffer Size (50 for now)
	char inBuff[inBufSize];			// char buffer for storing incoming usernames/requests
	int outBufSize = 256;			// set outgoing buffer size (256 for now)
	char outBuff[outBufSize];		// char buffer for storing outgoing usernames/requests

	// 1 - Read in name/key file to data table
	// Get File Name
	while (){
		cout << "Enter the key file name: ";
		cin >> keyfile;
		cout << "\nAttempting to open" << keyfile << "...\n";

		ifstream fin(keyfile);		// attempt to open file with provided name

		if (!fin) { 	// file did not open successfully, try again
			cout << "File not found. Please try again. \n\n";
		}
		else {			// file opened successfully!
			cout << "File found! Loading data to server memory...\n";
			// Load data from file to data table
		// Program assumes data file lines will include username followed by the corresponding key separated by white space
		// these two strings do not need to be on the same line but they must occur in order "username key username key etc..."
			vector<TableLine> KeyTable;		// KeyTable is a vector of type TableLine that holds data from the file
			while (fin >> dataholder){			// loop while able to read in a new string from fin
				TableLine newLine;				// create new TableLine struct to add to KeyTable
				// convert username string to all lowercase to avoid case sensitivity issues when searching for a match later
				transform(dataholder.begin(), dataholder.end(), dataholder.begin(), ::tolower);
				newLine.uname = dataholder;		// set uname to first string pulled
				fin >> dataholder;				// read in
				// keys are case sensitive and should remain as they are provided. We do not search by key values in this program.
				newLine.key = dataholder;		// set key to next string pulled
				KeyTable.push_back(newLine);	// add newLine to the data table
				tablesize++;					// increment the table size
			}
			break;					// continue to setting port number
		}
	}

	// Print KeyTable (for testing purposes)
	cout << "\n=========================================\n";
	cout << "KeyTable:";
	cout << "\n=========================================\n";
	for (int i = 0; i < tablesize; i++) {
		cout << KeyTable[i].uname << "\t" << KeyTable[i].key << "\n";
	}

	// Get Port Number
	while() {
		cout << "Enter server port number (greater than 1024): ";
		cin >> PORTNUM;		// read in port number
		if(PORTNUM > 1024)	// test port number is high enough
			break;			// if proper port number then continue to setting up socket
		else
			cout << "\nImproper port number selected, try again...\n\n";
	}

	// 2 - Create a socket()
	// 3 - Bind an address to that socket
	// 4 - Set up a buffer size for that socket
	if ((s= establish(PORTNUM)) < 0){ 		/* plug in the phone */
			perror("establish");
			exit(1);
	}

	// 5 - Wait for incoming calls
	bool keepListening = true;
	while (keepListening) { 								/* loop for incoming requests */
			// 6 - Accept incoming call and create a new socket
			if ((t= get_connection(s)) < 0){ 	/* get a connection */
				if (errno == EINTR) 			/* EINTR might happen on accept(), */
					continue; 					/* try again */
				perror("accept"); 				/* bad */
				exit(1);
			}

			// 7 - Reply to client with requested public key (if it exists)
			// read from client, look up requested information, respond to client
			bzero(inBuff, inBufSize);									// clear contents of inBuff
			read_data(t, inBuff, inBufSize);							// read in data from socket t, set byteCount to byte count from read_data()
			string dIn(inBuff);											// convert char to string
			transform(dIn.begin(), dIn.end(), dIn.begin(), ::tolower);	// convert string to all lowercase
			// print string for testing
			cout << "\nUsername received: " << dIn << "\n";
			// if termination command is received shut down the server
			if (dIn == "terminate") {
				// 9 - Terminate SERVER (this program)
				closeServer(s, t);		// call closeServer function to shut down the server
				// program should terminate when closeServer is called, following code to ensure finite loop
				keepListening = false;
				break;
			}
			bzero(outBuff, outBufSize);									// cleaer the contents of the outgoing buffer
			// else continue to look up provided username in the KeyTable
			for (int i = 0; i < tablesize; i++) {
				if (KeyTable[i].uname == dIn) {		// if a match is found
					strcpy(outBuff, (KeyTable[i].key).c_str());	// string copy key value at that location to the outgoing buffer
					break;										// match was found, no need to keep searching
				}
			}

			write_data(t, outBuff, outBufSize);					// send response to client (empty string if no match is found)

			// 8 - Hang up (close the socket)
			close(t);			// socket t is the "call", s is the "phone"
		}

	return 0;
}

// From BSD Primer, page 2: establish() - code to establish a socket
int establish(unsigned short portnum){
	int MAXHOSTNAME = 32;
	char myname[MAXHOSTNAME+1];
	int s;
	struct sockaddr_in sa;
	struct hostent *hp;

	memset(&sa, 0, sizeof(struct sockaddr_in)); 		/* clear our address */
	gethostname(myname, MAXHOSTNAME); 					/* who are we? */
	hp= gethostbyname(myname); 							/* get our address info */

	if (hp == NULL){ 									/* we don't exist !? */
		return(-1);
	}

	sa.sin_family= hp->h_addrtype; 						/* this is our host address */
	sa.sin_port= htons(portnum); 						/* this is our port number */

	// 2 - Create a socket()
	if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0){ 		/* create socket */
		return(-1);
	}

	// 3 - Bind an address to that socket
	if (bind(s,&sa,sizeof(struct sockaddr_in)) < 0){
		close(s);
		return(-1); 									/* bind address to socket */
	}

	// 4 - Set up a buffer size for that socket
	listen(s, 3); 										/* max # of queued connects */
	return(s);
}

// From BSD Primer, page 2: get_connection - function that can be used to accept a connection on a socket that has been created using establish() (see above)
// wait for a connection to occur on a socket created with establish()
int get_connection(int s) {
	int t; 								/* socket of connection */
	if ((t = accept(s,NULL,NULL)) < 0){ /* accept connection if there is one */
		return(-1);
	}
	return(t);
}

// From BSD Primer, page 4: read_data()- simple function to read a given number of characters into a buffer
int read_data(int s /* connected socket */,	char *buf /* pointer to the buffer */, int n /* number of characters (bytes) we want */){
	int bcount; 								/* counts bytes read */
	int br; 									/* bytes read this pass */
	bcount= 0;
	br= 0;

	while (bcount < n){ 						/* loop until full buffer */
		if ((br= read(s,buf,n-bcount)) > 0){
			bcount += br; 						/* increment byte counter */
			buf += br; 							/* move buffer ptr for next read */
		}else if (br < 0){ 						/* signal an error to the caller */
			return(-1);
		}
	}
	return(bcount);
}

// Modified from read_data method provided in BSD Primer
int write_data(int s /* connected socket */,	char *buf /* pointer to the buffer */, int n /* number of characters (bytes) we want */){
	int bcount; 								/* counts bytes written */
	int br; 									/* bytes written this pass */
	bcount= 0;
	br= 0;

	while (bcount < n){ 						/* loop until empty buffer */
		if ((br= write(s,buf,n-bcount)) > 0){
			bcount += br; 						/* increment byte counter */
			buf += br; 							/* move buffer ptr for next write */
		}else if (br < 0){ 						/* signal an error to the caller */
			return(-1);
		}
	}
	return(bcount);
}

void closeServer(int s, int t) {
	cout << "Termination command received. Shutting down server.\n";
	close(t);	// "hang up the phone"
	close(s);	// "unplug the phone"
	exit(0);	// "turn off the lights"
}
