#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
#include <ctype.h>

/** Port number used by my server */
#define PORT_NUMBER "26146"

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

sem_t s_array[26];
int vars[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/** handle a client connection, close it when we're done. */
void handleClient( int sock ) {
  // Here's a nice trick, wrap a C standard IO FILE around the
  // socket, so we can communicate the same way we would read/write
  // a file.
  FILE *fp = fdopen( sock, "a+" );
  
  // Prompt the user for a command.
  fprintf( fp, "cmd> " );

  char s[60];//get whole string 
  while (fgets( s, 60, fp) != NULL) {
	char cmd[11];//get command 
	char var1[11];//get first variable 
	char var2[11];//get second variable 
	int value = 0;//value for if second variable is number 
	sscanf(s, "%s %s %s", cmd, var1, var2);//scan the string to get command and variables 
	if (strcmp(cmd, "set") == 0) {
	  //get value of var2 in terms of ascii 
	  int len = strlen(var2);
	  for(int i = 0; i < len; i++){
		value = value * 10 + ( var2[i] - '0' );
	  }
	  //if var2 is a number 
	  if(isdigit(var2[0])) {
	    int idx = var1[0] - 97;//get index of the character 
	    sem_wait(&s_array[idx]);//wait on semaphore of that index
	    vars[idx] = value;//set the value of the element in that index to the value 
	    sem_post(&s_array[idx]);//release on semaphore of that index
	  } else {
		//if var2 is not a number
		int v1idx = var1[0] - 97;//get index of the character 
		int v2idx = value - 49;//get index of var2
		sem_wait(&s_array[v1idx]);//wait on semaphore of that index
		sem_wait(&s_array[v2idx]);//wait on semaphore of that index 
		vars[v1idx] = vars[v2idx];//set the value of the element in that index to the value in the index of var2
	    sem_post(&s_array[v2idx]);//release on semaphore of that index
		sem_post(&s_array[v1idx]);//release on semaphore of that index
	  }
	} else if (strcmp( cmd, "quit" ) == 0 ) {
	  // Close the connection with this client.
      fclose( fp );
    } else if ( strcmp( cmd, "print") == 0) {
	  int idx = var1[0] - 97;//get index of the character 
	  fprintf(fp, "%d\n", vars[idx]);//print value in the index of the character 
	} else if(strcmp(cmd, "add") == 0) {
	  //get value of var2 in terms of ascii
	  int len = strlen(var2);
	  for(int i = 0; i < len; i++){
		value = value * 10 + ( var2[i] - '0' );
	  }
	  //if var2 is a number 
	  if(isdigit(var2[0])) {
	    int idx = var1[0] - 97;//get index of the character 
	    sem_wait(&s_array[idx]);//wait on semaphore of that index 
	    vars[idx] += value;//add value to the element in the index of the character 
	    sem_post(&s_array[idx]);//release on semaphore of that index
	  } else {
		//if var2 is not a number
		int v1idx = var1[0] - 97;//get index of the character 
		int v2idx = value - 49;//get index of var2
		sem_wait(&s_array[v1idx]);//wait on semaphore of that index 
		sem_wait(&s_array[v2idx]);//wait on semaphore of that index 
		vars[v1idx] += vars[v2idx];//add element of index of var2 to the element in the index of the character 
	    sem_post(&s_array[v2idx]);//release on semaphore of that index
		sem_post(&s_array[v1idx]);//release on semaphore of that index
	  }
	} else if(strcmp(cmd, "subtract") == 0) {
	  //get value of var2 in terms of ascii
	  int len = strlen(var2);
	  for(int i = 0; i < len; i++){
		value = value * 10 + ( var2[i] - '0' );
	  }
	  //if var2 is a number 
	  if(isdigit(var2[0])) {
	    int idx = var1[0] - 97;//get index of the character 
	    sem_wait(&s_array[idx]);//wait on semaphore of that index 
	    vars[idx] -= value;//subtract value from the element in the index of the charater 
	    sem_post(&s_array[idx]);//release on semaphore of that index
	  } else {
		//if var2 is not a number
		int v1idx = var1[0] - 97;//get index of the character 
		int v2idx = value - 49;//get index of var2
		sem_wait(&s_array[v1idx]);//wait on semaphore of that index 
		sem_wait(&s_array[v2idx]);//wait on semaphore of that index 
		vars[v1idx] -= vars[v2idx];//subtract element of var2 index from the element in the index of the charater 
	    sem_post(&s_array[v2idx]);//release on semaphore of that index
		sem_post(&s_array[v1idx]);//release on semaphore of that index
	  }
	} else if(strcmp(cmd, "multiply") == 0) {
	  //get value of var2 in terms of ascii
	  int len = strlen(var2);
	  for(int i = 0; i < len; i++){
		value = value * 10 + ( var2[i] - '0' );
	  }
	  //if var2 is a number 
	  if(isdigit(var2[0])) {
	    int idx = var1[0] - 97;//get index of the character 
	    sem_wait(&s_array[idx]);//wait on semaphore of that index 
	    vars[idx] *= value;//multiply value to the element in the index of the charater 
	    sem_post(&s_array[idx]);//release on semaphore of that index
	  } else {
		//if var2 is not a number
		int v1idx = var1[0] - 97;//get index of the character 
		int v2idx = value - 49;//get index of var2
		sem_wait(&s_array[v1idx]);//wait on semaphore of that index 
		sem_wait(&s_array[v2idx]);//wait on semaphore of that index 
		vars[v1idx] *= vars[v2idx];//multiply element of var2 index to the element in the index of the charater 
	    sem_post(&s_array[v2idx]);//release on semaphore of that index
		sem_post(&s_array[v1idx]);//release on semaphore of that index
	  }
	} else if(strcmp(cmd, "divide") == 0) {
	  //get value of var2 in terms of ascii
	  int len = strlen(var2);
	  for(int i = 0; i < len; i++){
		value = value * 10 + ( var2[i] - '0' );
	  }
	  //if var2 is a number 
	  if(isdigit(var2[0])) {
		if(value == 0) {
		  fprintf(fp, "%s\n", "Divide by zero");//print divide by zero
		  fprintf( fp, "cmd> " );//get next command
		  continue;//continue iterating to while loop 
		}
	    int idx = var1[0] - 97;//get index of the character 
	    sem_wait(&s_array[idx]);//wait on semaphore of that index 
	    vars[idx] /= value;//divide value from the element in the index of the charater 
	    sem_post(&s_array[idx]);//release on semaphore of that index
	  } else {
		//if var2 is not a number
		int v1idx = var1[0] - 97;//get index of the character 
		int v2idx = value - 49;//get index of var2
		if(vars[v2idx] == 0) {
		  fprintf(fp, "%s\n", "Divide by zero");//print divide by zero
		  fprintf( fp, "cmd> " );//get next command
		  continue;//continue to while loop. 
		}
		sem_wait(&s_array[v1idx]);//wait on semaphore of that index 
		sem_wait(&s_array[v2idx]);//wait on semaphore of that index 
		vars[v1idx] /= vars[v2idx];//divide element of var2 index from the element in the index of the charater 
	    sem_post(&s_array[v2idx]);//release on semaphore of that index
		sem_post(&s_array[v1idx]);//release on semaphore of that index
	  }
	} else {
		fprintf(stderr, "%s\n", "Invalid Command.");//print invalid command 
	}
    // Prompt the user for the next command.
    fprintf( fp, "cmd> " );
    fflush( fp );
  }
}

//thread starter function
void *t_function(void *arg) {
  int sock = *(int *) arg;//make integer of socket. 
  handleClient(sock);//call handle client function to handle the client request 
  pthread_exit(NULL);//exit the thread 
}

//main
int main() {
  // Prepare a description of server address criteria.
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( NULL, PORT_NUMBER, &addrCriteria, &servAddr) )
    fail( "Can't get address info" );

  // Try to just use the first one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Create a TCP socket
  int servSock = socket( servAddr->ai_family, servAddr->ai_socktype,
                         servAddr->ai_protocol);
  if ( servSock < 0 )
    fail( "Can't create socket" );

  // Hopefully this will stop the occasional "Can't bind socket"
  if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (int []){1},
                 sizeof(int)) == -1)
    fail( "Can't set socket options" );

  // Bind to the local address
  if ( bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) != 0 )
    fail( "Can't bind socket" );
  
  // Tell the socket to listen for incoming connections.
  if ( listen( servSock, 5 ) != 0 )
    fail( "Can't listen on socket" );

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  // Fields for accepting a client connection.
  struct sockaddr_storage clntAddr; // Client address
  socklen_t clntAddrLen = sizeof(clntAddr);
  
  //initialize the array of semaphores. 
  for(int i = 0; i < 26; i++) {
	sem_init(&s_array[i], 0, 1);
  }

  pthread_t thread;//make new threaf
  while ( true ) {
    // Accept a client connection.
    int sock = accept( servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
	int *socketPtr = &sock;//make pointer of the socket. 
	
	if ( pthread_create( &thread, NULL, t_function, (void *) socketPtr) != 0 ) {
	  fail( "Can't create a child thread" );//create thread. If the thread is not created, fail the program with the message. 
	}

	pthread_detach(thread);//detach thread. 
  }

  // Stop accepting client connections (never reached).
  close( servSock );

  return 0;
}
