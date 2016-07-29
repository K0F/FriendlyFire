/*****************************************************************************

   File: HPCP.C              Version 1.0                 Date: 5/25/98   

   This is a dd copy utility for UNICOM

	This is a CLIENT, Talking to multiple servers
   
      Intended for use with the following:
      		unix (linux) machine using unicom device drivers.

		Compile with:
				gcc hpcp.c -o hpcp
            
      The revision history is shown below:

            Rev 1.0 5/25/98  Initial version
				
  
      The following functions are in place, with the indicated
   last time of modification:
		
   calling flags:
      -v Set verbose flag to 1, equiv to -d 1
      -f <filename> input filename
      -h <host_addr> Host address or name of destination
      -p <host_port> Port number or service name, must also have -h
      -d <0|1|2> Debug modes, -d 0 eqiv to no verbose, -d 1 equiv to verbose
               
   outputs information to stdout, errors to stderr

   Note:    copys file one char at a time, for obvious reasons.
   
   TODO:    catch signals
				
	  
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <gdbm.h>

#define OPTIONS "vf:h:p:d:"
#define VERSION "Version 1.0, 5/25/98"

/* prototypes */
void msg(char *outstring);  

FILE *logf;
int verbose = 0;
unsigned int pid; /* this process ID */
char msgstr[120];
struct hostent *hostinfo;
struct servent *servinfo;

main(int argc, char *argv[])
{
int opt;
char infile[60];
char inhost[40];
int inport;
char inports[40];
char lhost[40];
int lport;
char lports[40];
int c,i,n;
int retval;
int hostfd;
FILE *fd;
/* socket variables */
int insockfd;
struct sockaddr_in address;
int len;
/* string variables */
char string[120];
char inchar;
char in_string[120];

/* init any variables needed */
sprintf(msgstr,"host port copy command, version %s\n",VERSION);
msg(msgstr);
if (argc < 4)
   {
   printf("\n Host port copy utility, copies a file to a host:port");
   printf("\n\t-v Set verbose flag to 1, equiv to -d 1");
   printf("\n\t-f <filename> input filename");
   printf("\n\t-h <host_addr> Host address or name of destination");
   printf("\n\t-p <host_port> Port number or service name, must also have -h");
   printf("\n\t-d <0|1|2> Debug modes, -d 0 eqiv to no verbose, -d 1 equiv to verbose");
   printf("\n\n");
   exit(0);
   }

/* first process the options */
sprintf(msgstr,"Options: ");
msg(msgstr);
verbose = 0;

while((opt=getopt(argc, argv, OPTIONS))!= -1)
   {
   switch(opt)
      {
      case 'v':
         verbose = 1;
         msg("Verbose ");
         break;
      case 'f':
         strncpy(infile,optarg,59);
         infile[59]='\0'; /* prevent buffer overruns */
         sprintf(msgstr,"Copying file %s ",infile);
         msg(msgstr);
         break;
      case 'h':
         strncpy(string,optarg,39);
         string[39]='\0'; /* prevent buffer overruns */
         sscanf(string,"%s",lhost);
         sprintf(msgstr,"Host is %s ",lhost);
         msg(msgstr);
         break;
      case 'p':
         strncpy(lports,optarg,39);
         lports[39]='\0'; /* prevent buffer overruns */
         sprintf(msgstr,"Port is %s ",lports);
         msg(msgstr);
         break;
      case 'd':
         sscanf(optarg,"%d",&verbose);
         sprintf(msgstr,"Verbose: %d ",verbose);
         msg(msgstr);
         break;
      case ':':
         sprintf(msgstr,"Option %c needs argument ",argv[optind-1]);
         msg(msgstr);
         break;
      case '?':
         sprintf(msgstr,"Unknown option %c ",optopt);
         msg(msgstr);
         break;
      }
   }
msg("\n");
/* !!! should process other non-option arguments here */

/* put a pid file in /var/run for management */
sprintf(string,"/var/run/hpcp.pid");
logf = fopen(string,"w");
fprintf(logf,"%d",getpid());
pid = getpid();
fclose(logf);

/* open up the device driver socket connection */
/* first convert to ip addr and service number if needed */
if ( strspn(lhost,"0123456789.")<strlen(lhost)  )
   {
   /* then it must be a name not an ip number */
   hostinfo = gethostbyname(lhost);
   if (!hostinfo)
      {
      sprintf(msgstr,"\nUnable to resolve address for host %s\n",lhost);
      msg(msgstr);
      exit(-1);
      }
   else
      strcpy(lhost,inet_ntoa(*(struct in_addr *)hostinfo -> h_addr));
   }
if ( strspn(lports,"0123456789")<strlen(lports)  )
   {
   /* then it must be a name not a number */
   servinfo = getservbyname(lports,"tcp");
   if (!servinfo)
      {
      sprintf(msgstr,"\nUnable to resolve port number for service  %s\n",lports);
      msg(msgstr);
      exit(-1);
      }
   else
      lport=ntohs(servinfo -> s_port);
   }
else
    sscanf(lports,"%d",&lport);

sprintf(msgstr,"Connecting to host %s, port %d\n",lhost,lport);
msg(msgstr);
hostfd = socket(AF_INET, SOCK_STREAM, 0);
address.sin_family = AF_INET;
address.sin_addr.s_addr = inet_addr(lhost);
address.sin_port = htons(lport);
len = sizeof(address);
retval = connect(hostfd, (struct sockaddr *)&address, len);
if (retval == -1)
   {
   sprintf(msgstr,
      "Unable to connect to host on %s:%d",lhost,lport);
   perror(msgstr);
   msg(msgstr);
   exit(-1);
   }

fd = fopen(infile,"r");
if (fd == NULL)
   {
   sprintf(msgstr,
      "Unable to open input file %s",infile);
   perror(msgstr);
   msg(msgstr);
   exit(-1);
   }

/* now start reading the input file */
i = 0;
while(1==1)
   {
   c = fgetc(fd);
   if (c == EOF)
      break;
   i++;

   if (verbose >= 2)
      printf("char %c (%x)\n",c,c);

   retval = write(hostfd, &c, 1);
   if (retval == -1)
      {
      sprintf(msgstr,"Unable to write to host\n");
      msg(msgstr);
      }

   }


sprintf(msgstr,"transferred %d chars\n",i);
msg(msgstr);

close(hostfd);
fclose(fd);
}

/********************************Functionsville ***********************/
/**********************************************************************
*
* Message output routine
* Takes input string, and outputs it to screen (if verbose)
*
**********************************************************************/
void msg(char *outstring)
{
extern int verbose;
extern char msgstr[];
int i=0;

if (verbose)
   {
   printf("%s",outstring);
   }
}
