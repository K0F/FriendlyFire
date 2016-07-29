/*****************************************************************************

   File: LOGICDD.C              Version 1.0                 Date: 2/1/98   

      This is the device driver for the logic system
      	Modified to use INET socket calls
	This is a SERVER, able to accept multiple clients

      Intended for use with the following:
      		unix (linux) machine 		

		Compile with:
				gcc -o logicdd logicdd.c -lgdbm

      The revision history is shown below:

            Rev 1.0 2/1/98  Initial version
  
      The following functions are in place, with the indicated
   last time of modification:
   
		
   calling format:
   	logicdd [-d0 | -d1 | -d2] <port_number> 
			where: -d0 is no debugging, errors only
					-d1 is some debugging
					-d2 is serious debugging (i.e., lots)

   outputs information to stdout, errors to stderr

	  
*****************************************************************************/
/* defines */
#define VER "1.0, 2/1/98"
#define _POSIX_SOURCE 1	//POSIX compliant code

/* includes */
#include <stdio.h>
#include <fcntl.h>	// for file open functions
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <gdbm.h>

int debug = 0;


int main(int argc, char *argv[])
{
int offset;
/* datbase variables */
char string[80];
char keystring[20];
char datstring[20];
GDBM_FILE db;
datum dbkey;
datum dbdata;
/* serial IO variables */
int fd;
int running = 1;	// control while
int i, j;
/* now the socket stuff */
int port;
int retval;
int server_sockfd, client_sockfd;
int server_len, client_len;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
fd_set readfds, testfds, wfds;
int afd, wfd;
int result;
int inchar;
struct timeval tv;
/* external variables */

/* first print the startup message */
fprintf(stdout,
 "Starting up the logic device driver, version %s, id=%ld...\n",VER,getpid());
if (argc < 2)
	{
	fprintf(stdout,
	 "\nCalling format is: logicdd [-d(0|1|2)] <port_number> ");
	fprintf(stdout,"\n\tNote:  port number is TCP/IP port");
	fprintf(stdout,"\n\t-d0 no debug (default), -d1 is debug mode");
	fprintf(stdout,"\n\t-d2 verbose debugging (use carefully!)\n\n");
	exit(0);
	}

/* now figure out the arguments */
if (argv[1][0] == '-')
	offset = 1;
if (argc == 3)
	if ( (argv[offset][1] == 'd') && (argv[offset][2] == '1') )
		{
		debug = 1;
		fprintf(stdout,"Debugging on, ");
		}
	else if ( (argv[offset][1] == 'd') && (argv[offset][2] == '2') )
		{
		debug = 2;
		fprintf(stdout,"Verbose Debugging on, ");
		}

db = gdbm_open("./logicdd.db", 512, GDBM_WRCREAT, 0x666, 0);
if (db == NULL)
   {
   fprintf(stdout, "Failed to open database file\n");
   exit(-1);
   }
 
unlink("server_socket");
/* now open the sockets */
sscanf(argv[1+offset],"%d",&port);
fprintf(stdout,"Using TCP/IP port %d \n",port);
server_sockfd = socket(AF_INET, SOCK_STREAM,0);
/* name the socket */
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = htonl(INADDR_ANY);
server_address.sin_port = htons(port);
server_len = sizeof(server_address);
retval = bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
retval = listen(server_sockfd, 16);

if ( retval  == -1)
	{
	perror("\nError listening to socket\n ");
	exit(-2);
	}

/* init the fd sets */
FD_ZERO(&readfds); //clear the readfds set
FD_SET(server_sockfd, &readfds);  //add server_sockfd to the readfds set
FD_SET(fd, &readfds);  //add the sio fd to the readfds set
	
/* now at this point we have the db file open, and the socket started */
/* enter the main processing loop */
while (running)
	{
	/* now look for an active socket */
	testfds = readfds;
	wfds = readfds;
	
	//printf("\nServer waiting: ");
	/* NOTE: this command blocks  until a char is avail for read */
	result = select(FD_SETSIZE, &testfds, (fd_set *)0,
	  (fd_set *)0, (struct timeval *)0 );
	if (result  == -1)
		{
		perror("\nError in select\n");
		exit(-3); // ??? break instead and close queues
		}	
	/* at this point we have had a fd set as active */
	/* find which one then service it*/
	for (afd = 0; afd < FD_SETSIZE; afd++)
		{
		if (FD_ISSET(afd, &testfds))
			{
			if (afd == server_sockfd)
			  {
			  client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
			  FD_SET(client_sockfd, &readfds);
			  if (debug == 2)
				  printf("\nAdded client on fd %d",client_sockfd);
			  }
			else	//then it must be a client
			  {
			  ioctl(afd, FIONREAD, &retval);
			  if (retval == 0)
			    {
			    close(afd);
			    FD_CLR(afd, &readfds);
			    printf("\nremoving client on fd %d",afd);
			    }
			  else  //then it is a legit client read
			    {
			    retval = read(afd, string, 79);
			    if (retval < 0)
			     {
			     printf("\nError %d receiving message\n",retval);
			     exit(-3);
			     }
             string[retval] = '\0';
             /* scan the string for the db actions */ 
             retval = sscanf(string,"%s %s",keystring,datstring);
             if (debug >= 2)
                fprintf(stdout,"\nscanned %s for %s:%s ",string,keystring,datstring);
             if (retval == 2)
               {
               /* then treat it as a store */
               dbkey.dptr = (void *)keystring;
               dbkey.dsize = strlen(keystring);
               dbdata.dptr = (void *)datstring;
               dbdata.dsize = strlen(datstring);
               retval = gdbm_store(db, dbkey, dbdata, GDBM_REPLACE);
               if (retval != 0)
                  {
                  fprintf(stdout,"Error updating db for key %s data %s\n",keystring,datstring);      
                  strcpy(string,"!!! ");
                  }
               else if (debug >= 1)
                  {
                  fprintf(stdout,"Stored key %s data %s\n",dbkey.dptr, dbdata.dptr);      
                  strcpy(string,"+ ");
                  }
               }
             else if (retval == 1)
               {
               /* then assume it is a fetch */
               dbkey.dptr = (void *)keystring;
               dbkey.dsize = strlen(keystring);
               dbdata = gdbm_fetch(db, dbkey);
               if (dbdata.dptr != NULL)
                  {
                  sprintf(string,"%s %s",keystring,dbdata.dptr);
                  free(dbdata.dptr);
                  if (debug >= 1)
                    fprintf(stdout,"key %s retrieved data %s\n",keystring,dbdata.dptr);
                  }
               else
                  {
                  strcpy(string,"- ");
                  if (debug >= 1)
                     fprintf(stdout,"key %s retrieved no data \n",keystring);
                  }
               }
             else
               {
               strcpy(string,"? ");
               printf("Error - unable to interpret %s\n",keystring);
               }
			    retval = write(afd, string, strlen(string));
			    if (retval <= 0)
			     {
			     fprintf(stdout,"\nError sending message to client %d\n",afd);
			     exit(-5);
			     }
			    }
			  }
			}
		}

	}	//end of while running

/* cleanup */
gdbm_close(db);
fprintf(stdout,"\n");
exit (0);

}	//end of main
/*******************************Functionsville******************************/
/************************************************************/
/* Thats it !                                               */
/************************************************************/
