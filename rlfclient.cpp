//============================================================================
// Name        : rlfclient.cpp
// Author      : Ryan Farrell
// Version     :
// Copyright   : Your copyright notice
// Description : Client program for Assignment 2
//		***Contains code provided in Explanations2.pdf and
//		BSD_Sockets_A_Quick_And_Dirty_Primer.pdf***
//============================================================================

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

int establish (unsigned short);
int call_socket(char *, unsigned short);
int read_data(int, char *, int);
int write_data(int, char *, int);

int main() {
	int s;
	unsigned short portnum = 0;
	string username;
	string realHostName;
	gethostname(realHostName, 32);
	string hname;
	int outBufSize = 50;
	char outBuff[outBufSize];
	int inBufSize = 256;
	char inBuff[inBufSize];


	// 1 - Prompt user for server host name port number
	while () {
		cout << "Input the server HOST NAME: ";
		cin >> hname;
		if (hname == realHostName)
			break;			// if hname matches realHostName then break this loop and continue to next step
		else {
			cout << "\n\n=====================================\n";
			cout << "Unexpected host name! Try again.\n";				// keep looping
			cout << "(HOST NAME should be the same as this machine)\n";	// hint the user to the correct answer
			cout << "=====================================\n\n";
		}
	}

	while () {
		cout << "Input the server PORT NUMBER: ";
		cin >> portnum;
		if (portnum > 1024)
			break;
		else
			cout << "\nTry a larger port number...\n";
	}

	// 2 - Prompt user for a username
	cout << "\nInput the USERNAME that you are requesting a key for: ";
	cin >> username;
	cout << "\n\n";
	// assume user entered the username they are looking for correctly.
	// user may be looking for a username that does not exist and that is OK.

	cout << "Establishing a socket...\n"
	// 3 - Create a socket()
	// 4 - Bind an address to that socket
	// 5 - Set up a buffer size for that socket
	if ((s= establish(portnum)) < 0){ 		/* plug in the phone */
			perror("establish");
			exit(1);
	}

	cout << "Socket established.\n";
	cout << "Calling server...\n";

	// 6 - Connect socket to the server
	int call_socket(hname, portnum);

	cout << "Connected to server.\n";
	cout << "Sending request to server...\n";

	// 7 - Send username to server
	strcpy(outBuff, username.c_str());		// convert string username into char[] outBuff
	write_data(s, outBuff, outBufSize);		// write data to the socket

	// 8 - Wait for a response from the server, read() response
	cout << "Waiting for a response from the server...\n";
	read_data(s, inBuff, inBufSize);		// read data from the socket

	// 9 - Close the socket
	close(s);

	// 10 - cout server's response (public key or null response)
	string keyReceived(inBuff);								// convert char to string
	cout << "The public key for use " << username << " is " << keyReceived << "\n\n";

	// 9 - Terminate CLIENT (this program)
	return 0;
}

// From BSD Primer: code to establish a socket; originally from bzs@bu-cs.bu.edu
int establish(unsigned short portnum){
	int MAXHOSTNAME = 32;
	char myname[MAXHOSTNAME+1];
	int s;
	struct sockaddr_in sa;
	struct hostent *hp;
	memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
	gethostname(myname, MAXHOSTNAME); /* who are we? */
	hp= gethostbyname(myname); /* get our address info */
	if (hp == NULL) /* we don't exist !? */
		return(-1);
	sa.sin_family= hp->h_addrtype; /* this is our host address */
	sa.sin_port= htons(portnum); /* this is our port number */
	if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
		return(-1);
	if (bind(s,&sa,sizeof(struct sockaddr_in)) < 0) {
		close(s);
		return(-1); /* bind address to socket */
	}
	listen(s, 1); /* max # of queued connects */
	return(s);
}

//page 4: call_socket() - function that calls a particular port number on a particular host
int call_socket(char *hostname, unsigned short portnum){
	struct sockaddr_in sa;
	struct hostent *hp;
	int a, s;

	if ((hp= gethostbyname(hostname)) == NULL){ 			/* do we know the host's */
		errno= ECONNREFUSED; 								/* address? */
		return(-1); 										/* no */
	}

	memset(&sa,0,sizeof(sa));
	memcpy((char *)&sa.sin_addr,hp->h_addr,hp->h_length); 	/* set address */
	sa.sin_family= hp->h_addrtype;
	sa.sin_port= htons((u_short)portnum);
	if ((s= socket(hp->h_addrtype,SOCK_STREAM,0)) < 0){ 	/* get socket */
		return(-1);
	}

	if (connect(s,&sa,sizeof sa) < 0){ 						/* connect */
		close(s);
		return(-1);
	}
	return(s);
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
