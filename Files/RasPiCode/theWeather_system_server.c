/*******************************************************************************\
| rasPiServer.c
| Author: Todd Sukolsky
| Initial Build: 3/18/2013
| Last Revised:4/25/2013
|================================================================================
| Description: This is a server implemented on the RasPi used to communicate
|		with theWeather.system board.
|--------------------------------------------------------------------------------
| Revisions: 3/18-Initial Build
|	     3/19-Tweaked this to go into a function on positive listen and then fork and 
|		  exec. Initially had an issue with open pipes/blocking, fixed it though by
|		  closing every single pipe not being used in the master parent. See commits
|		  from earlier today. (2)Got the exec calls working with correct piping.
|		  (3) Tested hard, works well. Expanded buffer size but doesn't cause 
|		      issues. Just need to add the "Advanced feature"...thinking
|	     3/20-Cleaned code, added more comments. Made a makefile for this thing. If the port
|		  number is not given on execution, program will ask for it. Killed with ctrl-c.
|		 4/25- Tweaked to work with theWeather.system AVR board
|================================================================================
| *NOTES:(1) Basic socket info was provided by the instructor. Other information on 
|		  	 sockets can be found at ...
|		     http://www.microhowto.info/howto/listen_for_and_accept_tcp_connections_in_c.html
|
\*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

//How many requests can be sent into a pending state/queue
#define PENDING_REQUESTS 20

/*===============================*/
/*	Forward Declarations	 */
/*===============================*/
void error(const char *msg);
bool dealWithConnection(int socketHandle, const char *emailAddress);
	
/*===============================*/
/*      Main Program		 */
/*===============================*/

int main(int argc, char *argv[]){
	//Declare Variables
        int sockfd, newsockfd, portno;				//Server socket descriptor, client<->server socket descriptor, portno
        socklen_t clilen;					//length of socket name for client
        struct sockaddr_in serv_addr, cli_addr;
        int n;							//length used during read and write system calls
	char buffer[64];
	//If there are too few arguments provided on command line, say it needs a port number and ask.
        if (argc < 2) {
                fprintf(stderr,"ERROR, no port provided\n");
		printf("Provide port number: ");
		scanf("%d",&portno);				//get the port number
        } else{portno = atoi(argv[1]);}

	//Create server socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);		//create the socket
        if (sockfd < 0){error("ERROR opening socket");}
        bzero((char *) &serv_addr, sizeof(serv_addr));		//zero out server address
        serv_addr.sin_family = AF_INET;				//assign to TCP
        serv_addr.sin_addr.s_addr = INADDR_ANY;			//assign server address
        serv_addr.sin_port = htons(portno);			//assign port number
	 
        //Bind local address to server socket
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
     	sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
	
        //Now listen
  	listen(sockfd,PENDING_REQUESTS);
	 
	//THis is what should continue and contine and continue
	for (;;){
		printf("Server is ready...\n");
		//Accept a new client
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);

		//new sockfd has what we are going to be printing too. Can spawn a new process or whatever
		if (newsockfd < 0){error("ERROR on accept");}
		n=read(newsockfd,buffer,64);				//get what they are asking for
		if (n<0){error("ERROR writing to socket...");}	
		
		//There wasn't an error, we receieved some string from the client.
		else {
			bool successful=false;
			successful=dealWithConnection(newsockfd,buffer);
			if (!successful){error("Unable to complete request.");}
			bzero(buffer,64);
		}//end else
		close(newsockfd);
	 }//end infinite for
     close(sockfd);
     return 0; 
}



/*===============================*/
/*         Functions		 */
/*===============================*/

void error(const char *msg)
{
    perror(msg);
  // exit(1);
}

/*================================================================================================================*/

bool dealWithConnection(int socketHandle, const char *emailAddress){
	//Declare Variables.
	int pid, myPipe[2];		//process ids, pipes, lengths or read/writes
	char receiveBuffer[128];			//declare buffer

	if (pipe(myPipe)<0){error("Unable to initialize pipes."); return false;}

	//First fork to manage the email list, it  will call the other script
	pid=fork();
	if (pid<0){error("Unable to fork."); return false;}
	if (pid==0){//child
		//Close input, set output
		dup2(myPipe[1],1);
		dup2(myPipe[1],2);
		close(myPipe[0]);
		
		char *args[]={"/home/sukolsky/Documents/ManageEmails.py","-e",emailAddress,0};
		execv(args[0],args);
		error("Unable to exec communication program");
		return false;
	} else {	//parent
		//Close output, read input
		dup2(myPipe[0],0);
		close(myPipe[1]);
		
		//Wait for process to finish, no zombie process.
		waitpid(pid,NULL,0);
	}
	printf("Got email address:%s\n",emailAddress);
	int n=write(socketHandle,emailAddress,strlen(emailAddress));
	return true;
}//end function

/*================================================================================================================*/

		


