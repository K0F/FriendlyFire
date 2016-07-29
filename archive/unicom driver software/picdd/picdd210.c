/*****************************************************************************

   File: PICDD.C              Version 2.1                 Date: 4/12/98   

      This is the device driver  for the PICservo system
      Modified to use INET socket calls
      This is a SERVER, able to accept multiple clients

      Intended for use with the following:
            unix (linux) machine with picservo on a /dev/cuax port
      

      The revision history is shown below:

            Rev 1.0    12/18/97  Initial version
            Rev 1.1    12/19/97  Changed kd in init to 30,000
            Rev 1.1.1  12/24/97  Changed retval = listen(server_sockfd, 16)
            Rev 1.1.2  12/28/97  added command end on :
            Rev 1.2    12/29/97  Modified diagnostics, added log file
            Rev 1.3    1/11/98   Changed retval after trans to > 0,
               was >= 0
            Rev 2.0    3/13/98   Major upgrade / change in behaviour.
               Added ability to accept multiple socket clients,
               Separated pic input and output routines,
               Added ability to generate midi output from pic positions.
               Changed stop to soft stop
            Rev 2.1    4/12/98  Rebuild of logic flow to remove sio fd from
               select function, ignore SIGPIPE signal, added home commands,
               added ability to send special notes on limit switch changes

      The following functions are in place, with the indicated
   last time of modification:
   
         ahtoi                         10/11/88
         itoah                         10/11/88
         trans                         4/12/98
         pic_in                        4/12/98
         pic_out                       3/13/98
         midi_trans                    4/12/98

   calling format:
      picdd [-d0 | -d1] <port_number> <serial_device> 

   outputs information to stdout, errors to stderr

!!!todo - 
     disable output of midi but still run if no config file
          
     keeping track of number of bytes expected in return should take 
     into account group addresses as well.
     
*****************************************************************************/
/* defines */
#define VER "2.1, 4/12/98"
#define BAUD B19200  //fixed at 19200 Baud
#define _POSIX_SOURCE 1 //POSIX compliant code

/* includes */
#include <termios.h> // for serial port params
#include <stdio.h>
#include <fcntl.h>   // for file open functions
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>

/* external variables */
struct pic_vals
   {
   unsigned addr;
   unsigned enable;
   unsigned kp;
   unsigned kd;
   unsigned ki;
   unsigned il;
   unsigned cl;
   unsigned ol;
   unsigned el;
   unsigned sr;
   long pos;
   long vel;
   long accel;
   unsigned pwmvel;
   char pwmdir[10];
   unsigned char sb; /* status byte */
   unsigned char asb;   /* aux status byte */
   int mchan;  /* midi channel to broadcast on */
   int mstatnote; /* midi note to use for short status */
   unsigned char lim1;  /* limit switch 1 value */
   unsigned char lim2;  /* limit switch 2 value */
   int mlim1note; /* note to use for limit 1 value */
   int mlim2note; /* note to use for limit 2 value */
   long p0;    /* home position */
   long p1;    /* end position */
   long v0;    /* base velocity */
   long v1;    /* end velocity */
   int pn0;    /* home note by position */
   int pn1;    /* end note by position */
   int vv0;    /* home vel by velocity */
   int vv1;    /* end vel by velocity */
   float pm;   /* slope of note-position line */
   float pb;   /* intcpt of note-position line */
   float vm;   /* slope of velocity-velocity line */
   float vb;   /* intcpt of velocity-velocity line */
   int mnote;  /* midi note value calculated */
   int mvel;   /* midi velocity calculated */
   };
struct pic_vals pic[16];   //support upto 16 addresses
int pic_error;        /* picservo I/O subsytem's error flag */
int pic_status1;        /* picservo data location, first value */
int pic_status2;        /* picservo data location, second value */
int pic_statl;        /* picservo length of status return */
char pic_longstat[50];  /* picservo long status return string */
int pic_last_addr;   /* last picservo addressed */
int pic_last_nret;   /* number of return bytes expected from last cmd */
int debug = 0;
char command[80];
char response[80];
char string[80];
char lastcmd[20];
char midi_string[100];   /* support up to 10 note groups */
int stat_flag = 0;   /* used to force midi note out on regular stat cmd */

FILE *diags;

/* prototypes */
int trans(char message[]);
void pic_out(int sfd, char message[]);
int pic_in(int sfd);
int midi_trans(void);
int pic_init(void);
int ahtoi(char val);
char itoah(int ival);

int main(int argc, char *argv[])
{
int offset;
/* serial IO variables */
int fd;
int running = 1;  // control while
struct termios oldsio, newsio;
int i, j;
/* now the socket stuff */
int port;
int linger = 0;
int retval;
int server_sockfd, client_sockfd;
int server_len, client_len;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
fd_set readfds, testfds, wfds;
int afd, wfd, infd;
int result;
int inchar;
struct timeval tv;
char string2[20];
int maxfds = 16; /* start with 16 fds, add more if needed */
struct sigaction act;

/* external variables */
extern int pic_error;        /* pic I/O subsytem's error flag */
extern int pic_status1;        /* picservo data location, first value */
extern int pic_status2;        /* picservo data location, second value */
extern int pic_statl;        /* picservo length of status return */
extern char pic_longstat[];   /* picservo long status return string */

/* ignore the broken pipe signal */
act.sa_handler = SIG_IGN;
sigemptyset(&act.sa_mask);
act.sa_flags = 0;
if (sigaction(SIGPIPE, &act, 0) == -1)
   perror("Error ignoring SIGPIPE\n");

/* first print the startup message */
fprintf(stdout,
 "\n Starting up the pic device driver, version %s, id=%ld...\n",VER,getpid());
if (argc < 3)
   {
   fprintf(stdout,
    "Calling format is: picdd [-d(0|1)] <port_number> <serial_device>\n");
   fprintf(stdout,"\tNote: serial device name does not include path\n");
   fprintf(stdout,"\t      port number is TCP/IP port\n");
   fprintf(stdout,"\t-d0 no debug (default), -d1 is debug mode\n\n");
   exit(0);
   }

/* now figure out the arguments */
if (argv[1][0] == '-')
   offset = 1;
if (argc == 4)
   if ( (argv[offset][1] == 'd') && (argv[offset][2] == '1') )
      {
      debug = 1;
      fprintf(stdout,"Debugging on, writing to log file picdd.log\n");
      diags = fopen("./picdd.log","w+");
      }

if (pic_init() < 0)
   {
   printf("error in pic_init\n");
   exit(-1);
   }      

/* now open the serial device */
sprintf(string,"/dev/%s",argv[2+offset]);
fprintf(stdout,"Opening serial device %s, ",string);
fprintf(diags,"Opening serial device %s, ",string);
sscanf(argv[1+offset],"%d",&port);
fprintf(stdout,"Using TCP/IP port %d \n",port);
fprintf(diags,"Using TCP/IP port %d \n",port);
fd = open(string, O_RDWR | O_NOCTTY);
if (fd < 0)
   {
   perror(string);
   exit(-1);
   }
/* now get the old config and make a new one */
tcgetattr(fd, &oldsio); /* save the old attrs */
newsio.c_iflag = IGNPAR;
newsio.c_oflag = 0;
newsio.c_cflag = CS8 | CREAD | CLOCAL;
newsio.c_lflag = 0;  //non-canonical input
newsio.c_cc[VMIN] = 0;     //do not block waiting for 1st char 
newsio.c_cc[VTIME] = 1; //timeout after 100ms 
cfsetospeed(&newsio, BAUD);
cfsetispeed(&newsio, BAUD);
tcsetattr(fd, TCSANOW, &newsio);
while (read(fd, &i, 1) == 1);  /* clear out unspoken for characters */

unlink("server_socket");
/* now open the sockets */
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
FD_ZERO(&wfds); //clear the writefds set
FD_SET(server_sockfd, &wfds);  //add server_sockfd to the wfds set
   
/* now at this point we have the sio open, and the socket started */
/* enter the main processing loop */
while (running)
   {
   /* now look for an active socket */
   testfds = readfds;
      
   /* NOTE: this command blocks  until a char is avail for read */
   result = select(FD_SETSIZE, &testfds, (fd_set *)0,
     (fd_set *)0, (struct timeval *)0 );
   if (result  == -1)
      {
      perror("\nError in main select\n");
      exit(-3); // ??? break instead and close queues
      }   
   /* at this point we have had a fd set as active */
   /* find which one then service it */
   for (afd = 0; afd < maxfds; afd++)
      {
      if (FD_ISSET(afd, &testfds))
         {
         if (afd == server_sockfd)
            {
            client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
            FD_SET(client_sockfd, &readfds);
            FD_SET(client_sockfd, &wfds);
            if (debug >= 1)
               fprintf(stdout,"Added client on fd %d\n",client_sockfd);
            if (client_sockfd > maxfds)
               {
               maxfds = client_sockfd+1;
               if (debug >= 1)
                  fprintf(stdout,"Max file handles now %d\n",maxfds);
               }
            }
         else   /* then it must be a socket client */
            {
            ioctl(afd, FIONREAD, &retval);
            if (retval == 0)
               {
               close(afd);
               FD_CLR(afd, &readfds);
               FD_CLR(afd, &wfds);
               if (debug >= 1)
                  fprintf(stdout,"Removing client on fd %d\n",afd);
               }
            else  /* then it is a legit client read */
               {
               if (debug >= 1)
                  fprintf(stdout,"Reading from client on fd %d\n",afd);
               retval = read(afd, string, 79);               
               if (retval < 0)
                  {
                  fprintf(stdout,"\nErrno %d reading message on client %d\n",errno,afd);
                  perror("\nError in client read");
                  /* exit(-3); */
                  }
               infd = afd;   /* keep track of who sent it */
               string[retval]='\0'; /* to terminate the string */
               if (debug)
                  {
                  fprintf(stdout,"Received %d byte command:%s\n",
                     strlen(string), string);
                  fprintf(diags,"Received %d byte command:%s\n",
                     strlen(string), string);
                  }
               sscanf(string,"%s %d",string2,&i);        
               if (strcmp(string2,"ping") == 0)
                  {
                  if (debug) 
                     {
                     fprintf(stdout,"Attempting to determine location addr %d\n",i);
                     fprintf(diags,"Attempting to determine location addr %d\n",i);
                     }
                  /* best send the stat command to the pic then get response */
                  sprintf(string2,"stat %02d\n",i);
                  retval = trans(string2);
                  } /* end of if it is a ping */
               else  /* if it is not a ping, then send the command */
                  {
                  if (strcmp(string2,"stat") == 0)
                     stat_flag = 1; /* set the stat cmd flag to force midi out */               
                  /* now send it to pic */
                  /* first convert the command */
                  retval = trans(string);
                  }
               /* now output it */
               if (retval > 0) /* then it must be sent to the pic */
                  {
                  pic_out(fd,command);
                  if (pic_error < 0)
                     fprintf(stdout,"pic out error %d\n",pic_error);
                  /* now look for a sio response */
                  retval = pic_in(fd);
                  if (retval < 0)
                     fprintf(stdout,"pic_in error %d\n",retval);
                  else if (retval > 0)
                     {
                     /* if it is a stat or posit info, then trans to midi and send */            
                     retval = midi_trans();
                     if (retval <= -100)
                        fprintf(stdout,"Error %d on midi_trans\n",retval);
                     else if (retval > 0)
                        {
                        if (debug >= 1)
                           printf("midi command of %d bytes:%s\n",retval,midi_string); 
                        /* now go thru and write to all writeable clients */
                        tv.tv_sec = 0;
                        tv.tv_usec = 0; /* return immediately */
                        result = select(FD_SETSIZE, (fd_set *)0, &wfds, (fd_set *)0, &tv );
                        while (result  == -1)
                           {
                           perror("\nError in write select\n"); /* try again */
                           result = select(FD_SETSIZE, (fd_set *)0, &wfds, (fd_set *)0, &tv );
                           /* exit(-3); */ // ??? break instead and close queues
                           }
                        if (result == 0)
                           {
                           fprintf(stdout,"No writeable clients for message:%s\n",midi_string);
                           }
                        else
                           {   
                           /* at this point we have a writeable fd set as active */
                           /* find which one then service it*/
                           for (wfd = 0; wfd < maxfds; wfd++)
                              {
                              if (FD_ISSET(wfd, &wfds))
                                 {
                                 if ( wfd != server_sockfd )
                                    {
                                    /* then it was a non server client, write */
                                    if (debug)
                                       fprintf(stdout,"Sending message out to client %d\n",wfd);
                                    if (write(wfd, midi_string, strlen(midi_string)+1) <= 0)
                                       {
                                       fprintf(stdout,"\nError sending message to client %d\n",wfd);
                                       perror("\nError sending message to client\n");
                                       /* exit(-4); */
                                       }
                                    }
                                 }
                              }
                           } /* end of else */
                        }  /* end of else if (retval > 0) on midi_trans() */
                     }  /* end of else if (retval > 0) on pic_in() */
                  }

               } /* end of legit client read */
            } /* end of socket client */

         } /* end of if FD_ISSET */
      } /* end of for loop looking for set fd's */
   }  /* end of while running */

/* cleanup */
fprintf(stdout,"\n");
exit (0);

}  //end of main
/*******************************Functionsville******************************/
/***************************************************************/
/*    trans(char message[])                                    */
/*    translates the command message to the picservo subsystem.*/
/*    assumes the command is in the form <command> <addr> <...>*/
/* 12/19/97 - changed kd to 30000 in init                      */
/* 4/12/98  - added pic_last_nret to know # of bytes expected  */
/*            added home1 and home2 commands                   */
/***************************************************************/
/* translate the command */
int trans(char message[])
{
extern char command[];
extern char lastcmd[];
extern int pic_last_addr;   /* last picservo addressed */
extern int pic_last_nret;   /* number of return bytes expected from last cmd */
char cmd[20];
unsigned addr;
int retval;
int i;
int num;
long int ltemp;
int temp, temp2;
char ctemp;

retval = 0; //0 means no command to run, 1 = do run
pic_last_addr = -1;   /* no last addr, only set when an action command */
pic_last_nret = 0;   /* default to no bytes expected */

if (debug >= 2)
{
fprintf(stdout,"Translate:");
fprintf(diags,"Translate:");
for (i=0; i<80; i++)
     {
     if (message[i] == '\0')
     break;
     fprintf(stdout,"%x ",message[i]);
     fprintf(diags,"%x ",message[i]);
     }
fprintf(stdout,"\n");
fprintf(diags,"\n");
}

num = sscanf(message, "%s %d",cmd,&addr);
if (num < 2)
   {
   printf("\nerror: %s must have at least two fields, has %d\n",
          message,num);
   fprintf(diags,"\nerror: %s must have at least two fields, has %d\n",
          message,num);
   return(-1);
   }
/* find the addr or the next unused one */
for (i=0; i<16; i++)
   if ((pic[i].addr == addr)||(pic[i].addr == -1))
      break;
if (i>=16)
   {
   printf("\nerror: used up all storage locations\n");
   fprintf(diags,"\nerror: used up all storage locations\n");
   exit(-1);
   }
else if (pic[i].addr == -1)
   {
   pic[i].addr = addr;
   if (debug >= 2)
      {
      fprintf(stdout,"added address %d to pic[%d]\n",addr,i);
      fprintf(diags,"added address %d to pic[%d]\n",addr,i);
      }
   }
//at this point i is the offset into the info structure
//copy the command to the structure 
strcpy(lastcmd, cmd);   
if (strcmp(cmd,"nop") == 0)
   {
   sprintf(command,"%02x 0e;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"reset") == 0)
   {
   sprintf(command,"%02x 0f;",addr);
   pic_last_addr = addr;
   pic_last_nret = 0;
   retval = 1;
   }
else if (strcmp(cmd,"sleep") == 0)
   {
   sleep( (unsigned int)addr);
   retval = 0;
   pic_last_nret = 0;
   }
else if (strcmp(cmd,"csb") == 0)
   {
   sprintf(command,"%02x 0b;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"off") == 0)
   {
   sprintf(command,"%02x 17 02;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"rpos") == 0)
   {
   sprintf(command,"%02x 00;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"home1") == 0)
   {
   sprintf(command,"%02x 19 19;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"home2") == 0)
   {
   sprintf(command,"%02x 19 1a;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"init") == 0)
   {
   pic[i].kp = 1000;
   pic[i].kd = 30000;
   pic[i].ki = 1000;
   pic[i].il = 100000;
   pic[i].ol = 255;
   pic[i].cl = 0;
   pic[i].el = 512;
   pic[i].sr = 1;
   sprintf(command,"%02x d6 %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x;",
      addr,(0xff&pic[i].kp),(0xff00&pic[i].kp)>>8,(0xff&pic[i].kd),(0xff00&pic[i].kd)>>8,
      (0xff&pic[i].ki),(0xff00&pic[i].ki)>>8,(0xff&pic[i].il),(0xff00&pic[i].il)>>8,
      pic[i].ol,pic[i].cl,(0xff&pic[i].el),(0xff00&pic[i].el)>>8,pic[i].sr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"gain") == 0)
   {
   sprintf(command,"%02x d6 %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x;",
      addr,(0xff&pic[i].kp),(0xff00&pic[i].kp)>>8,(0xff&pic[i].kd),(0xff00&pic[i].kd)>>8,
      (0xff&pic[i].ki),(0xff00&pic[i].ki)>>8,(0xff&pic[i].il),(0xff00&pic[i].il)>>8,
      pic[i].ol,pic[i].cl,(0xff&pic[i].el),(0xff00&pic[i].el)>>8,pic[i].sr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"vmode") == 0)
   {
   sscanf(message, "%s %d %s",cmd,&addr,&pic[i].pwmdir);
   if (strcmp(pic[i].pwmdir,"rev") == 0)
      sprintf(command,"%02x 94 f6 %02x %02x %02x %02x %02x %02x %02x %02x;",
         addr,(0xff&pic[i].vel),(0xff00&pic[i].vel)>>8,(0xff0000&pic[i].vel)>>16,
         (0xff000000&pic[i].vel)>>24,(0xff&pic[i].accel),(0xff00&pic[i].accel)>>8,
         (0xff0000&pic[i].accel)>>16,(0xff000000&pic[i].accel)>>24);
   else 
      sprintf(command,"%02x 94 b6 %02x %02x %02x %02x %02x %02x %02x %02x;",
         addr,(0xff&pic[i].vel),(0xff00&pic[i].vel)>>8,(0xff0000&pic[i].vel)>>16,
         (0xff000000&pic[i].vel)>>24,(0xff&pic[i].accel),(0xff00&pic[i].accel)>>8,
         (0xff0000&pic[i].accel)>>16,(0xff000000&pic[i].accel)>>24);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"enable") == 0)
   {
   pic[i].enable = 1;
   sprintf(command,"%02x 17 %02x;",addr,0x05);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"addr") == 0)
   {
   sscanf(message, "%s %d %d %d",cmd,&addr,&temp,&temp2);
   sprintf(command,"%02x 21 %02x %02x;",addr,temp, temp2);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"traj") == 0)
   {
   sprintf(command,"%02x e4 1f %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x 00;",
      addr,(0xff&pic[i].pos),(0xff00&pic[i].pos)>>8,(0xff0000&pic[i].pos)>>16,(0xff000000&pic[i].pos)>>24,
      (0xff&pic[i].vel),(0xff00&pic[i].vel)>>8,(0xff0000&pic[i].vel)>>16,(0xff000000&pic[i].vel)>>24,
      (0xff&pic[i].accel),(0xff00&pic[i].accel)>>8,(0xff0000&pic[i].accel)>>16,(0xff000000&pic[i].accel)>>24);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"gtraj") == 0)
   {
   sprintf(command,"%02x e4 9f %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x 00;",
      addr,(0xff&pic[i].pos),(0xff00&pic[i].pos)>>8,(0xff0000&pic[i].pos)>>16,(0xff000000&pic[i].pos)>>24,
      (0xff&pic[i].vel),(0xff00&pic[i].vel)>>8,(0xff0000&pic[i].vel)>>16,(0xff000000&pic[i].vel)>>24,
      (0xff&pic[i].accel),(0xff00&pic[i].accel)>>8,(0xff0000&pic[i].accel)>>16,(0xff000000&pic[i].accel)>>24);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"go") == 0)
   {
   sprintf(command,"%02x 05;",addr);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"stop") == 0)
   {
   pic[i].enable = 1;
   sprintf(command,"%02x 17 %02x;",addr,0x09);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"stat") == 0)
   {
   sprintf(command,"%02x 13 ff;",addr);
   pic_last_addr = addr;
   pic_last_nret = 16;
   retval = 1;
   }
else if (strcmp(cmd,"pwm") == 0)
   {
   sscanf(message, "%s %d %s %d",cmd,&addr,&pic[i].pwmdir,&pic[i].pwmvel);
   if (strcmp(pic[i].pwmdir,"rev") == 0)
      sprintf(string,"%02x 24 c8 %02x;",addr,pic[i].pwmvel);
   else 
      sprintf(string,"%02x 24 88 %02x;",addr,pic[i].pwmvel);
   pic_last_addr = addr;
   pic_last_nret = 2;
   retval = 1;
   }
else if (strcmp(cmd,"vel") == 0)
   {
   num = sscanf(message, "%s %d %ld",cmd,&addr,&ltemp);
   pic[i].vel = ltemp;
   printf(" setting velocity for motor %d (%d) to %ld\n",
      pic[i].addr, i, pic[i].vel);
   pic_last_nret = 0;
   retval = 0;
   }
else if (strcmp(cmd,"pos") == 0)
   {
   num = sscanf(message, "%s %d %ld",cmd,&addr,&ltemp);
   pic[i].pos = ltemp;
   printf(" setting position for motor %d (%d) to %ld\n",
      pic[i].addr, i, pic[i].pos);
   pic_last_nret = 0;
   retval = 0;
   }
else if (strcmp(cmd,"accel") == 0)
   {
   sscanf(message, "%s %d %ld",cmd,&addr,&ltemp);
   pic[i].accel = ltemp;
   printf(" setting accelleration for motor %d (%d) to %ld\n",
      pic[i].addr, i, pic[i].accel);
   pic_last_nret = 0;
   retval = 0;
   }
else if (strcmp(cmd,"kp") == 0)
   {
   num = sscanf(message, "%s %d %d",cmd,&addr,&temp);
   pic[i].kp = temp;
   printf(" setting kp for motor %d (%d) to %d\n",
      pic[i].addr, i, pic[i].kp);
   pic_last_nret = 0;
   retval = 0;
   }
else if (strcmp(cmd,"kd") == 0)
   {
   num = sscanf(message, "%s %d %d",cmd,&addr,&temp);
   pic[i].kd = temp;
   printf(" setting kd for motor %d (%d) to %d\n",
      pic[i].addr, i, pic[i].kd);
   pic_last_nret = 0;
   retval = 0;
   }
else if (strcmp(cmd,"ki") == 0)
   {
   num = sscanf(message, "%s %d %d",cmd,&addr,&temp);
   pic[i].ki = temp;
   printf(" setting ki for motor %d (%d) to %d\n",
      pic[i].addr, i, pic[i].ki);
   pic_last_nret = 0;
   retval = 0;
   }
else
   {
   printf("\nerror: %s extracted from %s invalid command\n",cmd,message);
   fprintf(diags,"\nerror: %s extracted from %s invalid command\n",cmd,message);
   return -2;
   }
return retval;
}

/***************************************************************/
/*    pic_out(message)                                         */
/*    outputs a message to the picservo subsystem.             */
/*          Uses write to open device sfd                      */
/***************************************************************/
void pic_out(int sfd, char message[])
{
int i = 0;
int endi;
int ret_val = 0;
int error_val = 0;
int j = 0;
unsigned sum = 0;
int check_sum;
unsigned char out_array[23];
unsigned char in_array[50];
char c;
extern int pic_error;        /* pic I/O subsytem's error flag */
extern int pic_status1;        /* picservo data location, first value */
extern int pic_status2;        /* picservo data location, second value */
extern int pic_statl;        /* picservo length of status return */
extern char pic_longstat[];   /* picservo long status return string */

printf("\tEntering pic out routine with %s\n",message);
/* set the error flag and status to 0 each time the routine is called */
pic_error = 0;
pic_status1 = 0;
pic_status2 = 0;
pic_statl = 0;
pic_longstat[0]='\0';

/* first set up the out_array */
out_array[i++] = 0xaa;  /* start of command char */

/* next put in the message string, ignoring blank char's and nl and lf*/
while ((message[j] != '\0') && (i < 19) && (j < 50) && (message[j] != ';'))
   {
   if ( (message[j] != 0x20) 
   && (message[j] != 0x0d) 
   && (message[j] != ',') 
   && (message[j] != 0x0a) )
      {                       /* if the char is not a space */
      out_array[i] = ahtoi(message[j])<<4 | ahtoi(message[j+1]);
      sum += out_array[i];
      i++; j+=2;
      }
   /* if char is a space, ignore it */
   j++;
   }
if ((j >= 50) || (i >= 19))
   {
   pic_error = -24020;    /* an improper message string */
   return;                 /* terminate */
   }

/* next add the checksum */
out_array[i] = sum & 0xff;

if (debug>=2)
   {
   fprintf(stdout,"\tConverted message has %d bytes\n",i+1);
   for (j=0; j<=i; j++)
      fprintf(stdout,"\tchar %x (%d)\n",out_array[j],out_array[j]);
   fprintf(diags,"\tConverted message has %d bytes\n",i+1);
   for (j=0; j<=i; j++)
      fprintf(diags,"\tchar %x (%d)\n",out_array[j],out_array[j]);
   }

/* now output the string */
ret_val = write(sfd, out_array, i+1); //last char is in array[i]
if (ret_val < 0)
   error_val = -24200 + ret_val;

return;  
}

/***************************************************************/
/*    pic_in(file_handle)                                      */
/*    inputs a message from the picservo subsystem.            */
/*          Uses read from open device sfd                     */
/* 4/12/98  - added pic_last_nret to use # of bytes expected   */
/***************************************************************/
int pic_in(int sfd)
{
int i = 0;
int endi;
int ret_val = 0;
int error_val = 0;
int j = 0;
unsigned sum = 0;
int check_sum;
unsigned char in_array[50];
char c;
extern int pic_error;        /* pic I/O subsytem's error flag */
extern int pic_status1;        /* picservo data location, first value */
extern int pic_status2;        /* picservo data location, second value */
extern int pic_statl;        /* picservo length of status return */
extern char pic_longstat[];   /* picservo long status return string */
extern int pic_last_nret;   /* number of return bytes expected from last cmd */
int timeout = 0;

/* set the error flag and status to 0 each time the routine is called */
pic_error = 0;
pic_status1 = 0;
pic_status2 = 0;
pic_statl = 0;
pic_longstat[0]='\0';

/* now wait for some type of silly response */
i=0;
while (i<19)    /* get as many as 20 char's */
   {
   ret_val = read(sfd, &c, 1);   // get 1 char 
   if (ret_val <= 0)          /* then some kind of error or timeout happened */
      break;      
   else                       /* if not it must be a char */
      in_array[i++] = c;
   if (i == pic_last_nret)
      break;
   }
in_array[i] = '\0';
/* printf("retval = %d, i = %d\n",ret_val,i); */
/* if ((ret_val < 0)&&(errno != EAGAIN)) */
if (ret_val == 0)
   {
   pic_error = -24023;  /* timeout */
   pic_statl = 0;
   return pic_error;
   }

/* if here, then we have an array to process */
endi = i;      /* number of bytes read */
if (debug>=2)
   {
   fprintf(stdout,"\tRead back message has %d bytes\n",i);
   for (j=0; j<i; j++)
      fprintf(stdout,"\tchar %x (%d)\n",in_array[j],in_array[j]);
   fprintf(diags,"\tRead back message has %d bytes\n",i);
   for (j=0; j<i; j++)
      fprintf(diags,"\tchar %x (%d)\n",in_array[j],in_array[j]);
   }
if (debug)
   {
   fprintf(stdout,"Read back %d bytes \n",i);
   fprintf(diags,"Read back %d bytes \n",i);
   }
   
/* now figure out what the heck all this means */
pic_error = 0; /* clear the error  so we gots not timeout */
if (endi == 2) /* received two bytes */
   {
   /* check the checksum */
   pic_status1 = in_array[0];
   pic_status2 = in_array[1];
   pic_statl = endi;
   if (in_array[0] == in_array[1])
      pic_error = 0; /* then the checksum is ok */
   else
      {
      pic_error = -24022;
      /* try and clean up to get back on synch */
      while (read(sfd, &c, 1) == 1);
      return pic_error;
      }
   }
else
   {
   /* assume it is the upto 14 byte return */
   pic_status1 = in_array[0];
   pic_status2 = in_array[endi-1];  /* get the checksum */
   pic_statl = endi;
   j = 0;
   sum = 0;
   for (i=0; i<endi; i++)
      {
      if (i != (endi-1))
         sum += (unsigned)in_array[i];
      pic_longstat[j++] = itoah((in_array[i]&0xf0)>>4);
      pic_longstat[j++] = itoah(in_array[i]&0x0f);
      pic_longstat[j++] = ' ';
      }
   if ( (sum&0xff) != (unsigned)in_array[endi-1])
      {
      pic_error = -24022;
      while (read(sfd, &c, 1) == 1);
      return pic_error;
      }
   pic_longstat[j] = '\0';
   }
return pic_statl;  
}

/***************************************************************/
/*    midi_trans()                                             */
/*    determine if a midi note string is to be sent, prep it if*/
/*    so, returns -100 for error, 0 for nothing to do, 1 if    */
/*    midi string needs to be sent                             */
/* 4/12/98 - Added limit switch values                         */
/***************************************************************/
int midi_trans(void)
{
extern struct pic_vals pic[];   /* pic parameters */
extern int pic_error;        /* pic I/O subsytem's error flag */
extern int pic_status1;        /* picservo data location, first value */
extern int pic_status2;        /* picservo data location, second value */
extern int pic_statl;        /* picservo length of status return */
extern char pic_longstat[];   /* picservo long status return string */
extern int pic_last_addr;   /* last picservo addressed */
extern char midi_string[];       /* midi output bytes */
extern int stat_flag;   /* used to force midi note out on regular stat cmd */
int retval = 0;
unsigned b[20];
long cpos;
long cvel;
int mnote;
int mvel;
int i, j;
char string[20];

midi_string[0]='\0';    /* start with a blank string */

for (j=0; j<16; j++)
   if (pic[j].addr == pic_last_addr)
      break;   /* now j is the index into the pic structure */

if (pic_statl == 2)
   {
   /* then it was a short status, check a few things */
   if (debug >= 1)
      fprintf(stdout,"\tstatus byte is %x\n",pic_status1);
   sprintf(string,"%02x %02x %02x\n",(0x90|pic[j].mchan),pic[j].mstatnote,pic_status1);
   strcpy(midi_string,string);   /* add it to the building string */
   }
else if (pic_statl == 16)
   {
   /* then it was a long status, check the velocity and postion */
   sscanf(pic_longstat,"%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x"
   ,&b[1],&b[2],&b[3],&b[4],&b[5],&b[6],&b[7],&b[8],&b[9],
   &b[10],&b[11],&b[12],&b[13],&b[14],&b[15],&b[16]);
   // print the long status 
   cpos = (b[5]<<8)|b[4];   // first the pos
   cpos = cpos<<8|b[3];
   cpos = cpos<<8|b[2];
   cvel = (b[8]<<8+b[7]);
   fprintf(stdout,"\tPosit\tVel\tAD\tAux\ttype\n");
   fprintf(stdout,"\t%ld\t %d\t %d\t %d\t %d\n",
      cpos,cvel,b[6],b[9],(b[15]<<8+b[14]));
   
   /* put a ceiling and floor on the position and velocity */
   if (cpos > pic[j].p1)
      cpos = pic[j].p1;
   else if (cpos < pic[j].p0)
      cpos = pic[j].p0;
   if (cvel > pic[j].v1)
      cvel = pic[j].v1;
   else if (cvel < pic[j].v0)
      cvel = pic[j].v0;
      
   mnote = (int)( ((float)cpos*pic[j].pm)+pic[j].pb);
   mvel = (int)( ((float)cvel*pic[j].vm)+pic[j].vb);
   
   if ( (mnote != pic[j].mnote) || (mvel != pic[j].mvel) || (stat_flag == 1) )
      {
      /* then make up and send a new midi note value */
      sprintf(string,"%02x %02x %02x\n",(0x90|pic[j].mchan),mnote,mvel);
      strcpy(midi_string,string);   /* add it to the building string */
      pic[j].mvel = mvel;
      pic[j].mnote = mnote;
      stat_flag = 0;
      }
   /* add notes for limit switches, first lim1  */
   if ( (b[1]&0x20) != pic[j].lim1)
      {
      if ( (b[1]&0x20) == 0)
         sprintf(string,"%02x %02x %02x\n",
            (0x90|pic[j].mchan),pic[j].mlim1note,0);
      else 
         sprintf(string,"%02x %02x %02x\n",
            (0x90|pic[j].mchan),pic[j].mlim1note,0x7f);
      pic[j].lim1 = (b[1]&0x20);            
      strcat(midi_string,string);   /* add it to the building string */
      }
   /* now limit 2  */
   if ( (b[1]&0x40) != pic[j].lim2)
      {
      if ( (b[1]&0x40) == 0)
         sprintf(string,"%02x %02x %02x\n",
            (0x90|pic[j].mchan),pic[j].mlim2note,0);
      else 
         sprintf(string,"%02x %02x %02x\n",
            (0x90|pic[j].mchan),pic[j].mlim2note,0x7f);
      pic[j].lim2 = (b[1]&0x40);            
      strcat(midi_string,string);   /* add it to the building string */
      }
   /* !!! todo - add to string for status (static or changing???) */
   }

return strlen(midi_string);
}

/***************************************************************/
/* pic_init()                                                  */
/* initializes the midi part of the pic device, including      */
/* reading the config file picdd.cfg                           */
/* returns a 0 if ok, a value < 0 if an error occurred         */
/* 4/12/98 - Added limit switch values                         */
/***************************************************************/
int pic_init(void)
{
FILE *config_file;    /* configuration input file */
char string[80];
char in_string[80];
extern struct pic_vals pic[];  /* pic device structure */
char *p;
int i,j;
int retval = 0;

/* first initialize the pic structure  */
for (i=0; i<16; i++)
   pic[i].addr = -1;
   
/* now get the configuration file */
config_file = fopen("picdd.cfg","r");
if (config_file == NULL)
   {
   fclose(config_file);
   retval = -1;  /* file open error */
   }
else
{
/* if we are still here, we must read the file */
p = fgets(in_string,79,config_file);
while (p != NULL)
   {
   sscanf(in_string, "%s", string);
   if (strcmp(string,"addr") == 0)
      {
      sscanf(in_string,"%s %d",string,&j);
      /* now go thru and make a space in the pic[] struct for this guy */
      for (i=0; i<16; i++)
         {
         if ((pic[i].addr == j)||(pic[i].addr == -1))
            break;   /* now i is the matching one or the first unused */
         }
      if (i>=16)
         {
         fprintf(stdout,"\nerror: used up all storage locations\n");
         fprintf(diags,"\nerror: used up all storage locations\n");
         exit(-1);
         }
      pic[i].addr = j;
      }
   else if (strcmp(string,"mchan") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].mchan);
   else if (strcmp(string,"p0") == 0)
      sscanf(in_string,"%s %ld",string,&pic[i].p0);
   else if (strcmp(string,"p1") == 0)
      sscanf(in_string,"%s %ld",string,&pic[i].p1);
   else if (strcmp(string,"pn0") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].pn0);
   else if (strcmp(string,"pn1") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].pn1);
   else if (strcmp(string,"mchan") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].mchan);
   else if (strcmp(string,"v0") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].v0);
   else if (strcmp(string,"v1") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].v1);
   else if (strcmp(string,"vv0") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].vv0);
   else if (strcmp(string,"vv1") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].vv1);
   else if (strcmp(string,"mstatnote") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].mstatnote);
   else if (strcmp(string,"mlim1note") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].mlim1note);
   else if (strcmp(string,"mlim2note") == 0)
      sscanf(in_string,"%s %d",string,&pic[i].mlim2note);
   p = fgets(in_string,79,config_file);
   }
}

/* now do the calculations */
for (i=0; i<16; i++)
   {
   if (pic[i].addr != -1)
      {
      if ( (pic[i].p1-pic[i].p0) == 0)
         {
         fprintf(stdout,"pic config: addr %d, p1 - p0 = 0!!!\n",i);
         fprintf(diags,"pic config: addr %d, p1 - p0 = 0!!!\n",i);
         return -1;
         }
      else
         {
         pic[i].pm = (float)(pic[i].pn1-pic[i].pn0)/(float)(pic[i].p1-pic[i].p0);
         pic[i].pb = (float)pic[i].pn0 - pic[i].pm * pic[i].p0;
         pic[i].vm = (float)(pic[i].vv1-pic[i].vv0)/(float)(pic[i].v1-pic[i].v0);
         pic[i].vb = (float)pic[i].vv0 - pic[i].vm * pic[i].v0;
         }
      }
   }
   
/* write the diagnostics */
fprintf(diags,"\n\nPIC Initialization Data:");
for (i=0; i<16; i++)
   {
   if (pic[i].addr != -1)
      {
      fprintf(diags,"\nConfiguration values for pic[%d]\n",i);
      fprintf(diags,"mchan = %d\n",pic[i].mchan);
      fprintf(diags,"mstatnote = %d\n",pic[i].mstatnote);
      fprintf(diags,"mlim1note = %d\n",pic[i].mlim1note);
      fprintf(diags,"mlim2note = %d\n",pic[i].mlim2note);
      fprintf(diags,"p0 = %ld\n",pic[i].p0);
      fprintf(diags,"p1 = %ld\n",pic[i].p1);
      fprintf(diags,"pn0 = %d\n",pic[i].pn0);
      fprintf(diags,"pn1 = %d\n",pic[i].pn1);
      fprintf(diags,"pm = %f\n",pic[i].pm);
      fprintf(diags,"pb = %f\n",pic[i].pb);
      fprintf(diags,"v0 = %d\n",pic[i].v0);
      fprintf(diags,"v1 = %d\n",pic[i].v1);
      fprintf(diags,"vv0 = %d\n",pic[i].vv0);
      fprintf(diags,"vv1 = %d\n",pic[i].vv1);
      fprintf(diags,"vm = %f\n",pic[i].vm);
      fprintf(diags,"vb = %f\n",pic[i].vb);
      }
   }
fflush(diags);
fclose(config_file);

return retval;
}

/***************************************************************/
/*    itoah(ival)                                              */
/*    converts an integer to a single ascii hex character      */
/***************************************************************/
char itoah(ival)
int ival;
{

if ((ival >= 0x0) && (ival <= 0x9))
   return (char)(ival + 0x30);
if ((ival >= 0xa) && (ival <= 0xf))
   return (char)(ival + 0x37);
if (ival > 0xf)
   return '!';
}

/***************************************************************/
/*    ahtoi(val)                                               */
/*    converts a single ascii hex character to integer         */
/***************************************************************/
int ahtoi(val)
char val;
{
int ival;

if ((val >= '0') && (val <= '9'))
   return (val - 0x30);
if ((val >= 'a') && (val <= 'f'))
   return (val - 0x57);
if ((val >= 'A') && (val <= 'F'))
   return (val - 0x37);
}

/************************************************************/
/* Thats it !                                               */
/************************************************************/


