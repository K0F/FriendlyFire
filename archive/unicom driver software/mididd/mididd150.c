/*****************************************************************************

   File: MIDIDD.C              Version 1.5                 Date: 2/16/98   

      This is the device driver  for the MIDI system
      	Modified to use INET socket calls
	This is a SERVER, able to accept multiple clients

      Intended for use with the following:
      		unix (linux) machine with midiman on a /dev/cuax port		

		Compile with GCC Version 2.7.2.3 or better (use gcc -v to check)
      using command line: (must use the -funsigned-char option):
				gcc -o mididd -funsigned-char mididd.c
      
      May need to rlogin and run with:
       exec mididd -dx <port> <device> 
         to get baud rate settable to 115200 BAUD.

      The revision history is shown below:

            Rev 1.0 1/2/98  Initial version
				Rev 1.1 1/2/98  Added 350us delay to midiout
				Rev 1.2 1/18/98 Changed to always output 2 byte
										hex numbers, 0 padded
				Rev 1.3 2/1/98  Changed to print error number, ignore
										error on client read
				Rev 1.4 2/15/98 Changed to put \n at the end of screen and 
										socket outputs to aid in immediate flushing
				Rev 1.5 2/16/98 Modified for values sent from clients to be
                              sent to all other clients as well as out to 
                              serial port (midi hardware)  NOTE: these 
                              echo'd midi strings not processed thru midi_in.
  
      The following functions are in place, with the indicated
   last time of modification:
   
		midi_in		1/2/98
		midi_init	1/2/98
		strcvt		1/2/98 (unchanged from minicom versions)
		
   calling format:
   	mididd [-d0 | -d1 | -d2] <port_number> <serial_device>
			where: -d0 is no debugging, errors only
					-d1 is some debugging
					-d2 is serious debugging (i.e., lots)

   outputs information to stdout, errors to stderr

!!!todo - 
	  change to non-blocking socket calls???
	  restrict to less than FD_SETSIZE for select
	  address canonical mode on sio
	  input buad and port in config,
	  
*****************************************************************************/
/* defines */
#define VER "1.5, 2/16/98"
#define BAUD B115200
/* #define BAUD B38400 */ /* do this and use setserial to get to 115200 */
#define _POSIX_SOURCE 1	//POSIX compliant code

/* includes */
#include <termios.h>	// for serial port params
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
#include <errno.h>

#include </unisw/midi/mididd.h> //for mididd

int debug = 0;

char string[80];
char response[80];
char midistring[80];

/* prototypes */
int midi_in(int inchar);
int midi_init(void);
void strcvt(char *c);

int main(int argc, char *argv[])
{
int offset;
/* serial IO variables */
int fd;
int infd;
int running = 1;	// control while
struct termios oldsio, newsio;
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
 "\n Starting up the midi device driver, version %s, id=%ld...\n",VER,getpid());
if (argc < 3)
	{
	fprintf(stdout,
	 "\nCalling format is: mididd [-d(0|1|2)] <port_number> <serial_device>");
	fprintf(stdout,"\n\tNote: serial device name does not include path");
	fprintf(stdout,"\n\t      port number is TCP/IP port");
	fprintf(stdout,"\n\t-d0 no debug (default), -d1 is debug mode");
	fprintf(stdout,"\n\t-d2 verbose debugging (use carefully!)\n\n");
	exit(0);
	}

/* now check for the config file */
if (midi_init() != 0)
	printf("\nUnable to read mididd.cfg, using defaults\n");
			
/* now figure out the arguments */
if (argv[1][0] == '-')
	offset = 1;
if (argc == 4)
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

/* do the setserial to get the speed up */
/* sprintf(string,"/bin/setserial /dev/%s spd_vhi",argv[2+offset]); */
/* fprintf(stdout,"Setserial spd_vhi returned %d\n",system(string)); */

/* now open the serial device */
sprintf(string,"/dev/%s",argv[2+offset]);
fprintf(stdout,"Opening serial device %s, ",string);
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
newsio.c_lflag &= ~ICANON;	//non-canonical input
newsio.c_cc[VMIN] = 0;		//do not block waiting for 1st char 
newsio.c_cc[VTIME] = 0;	   //return immediately
cfsetospeed(&newsio, BAUD);
cfsetispeed(&newsio, BAUD);
tcsetattr(fd, TCSANOW, &newsio);

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
	
/* now at this point we have the sio open, and the socket started */
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
				  printf("Added client on fd %d\n",client_sockfd);
			  }
			else if (afd == fd)  //then it must be sio
			  {
			  retval = read(afd, &inchar, 1);
			  if (retval < 0)
			     {
			     printf("\nError %d receiving sio message\n",retval);
				  perror("Error in sio read\n");
			     exit(-3);
			     }
				retval = midi_in(inchar);
				if (retval <= -100)
					printf("Error %d on midi in, char = %x\n",retval,inchar);
				else if ((retval == -10)&&(debug == 2))
					printf("Rejected command on char %x due to granularity\n",inchar);
				else if ((retval == -1)&&(debug == 2))
					printf("Ignored byte %x\n",inchar);
				else if (retval > 0)
					{
			    	sprintf(midistring,"%02x %02x %02x",midi.in_buff[0],midi.in_buff[1],
						midi.in_buff[2]);
					if (debug >= 1)
						printf("midi command of %d bytes:%s\n",retval,midistring); 
					/* now go thru and write to all writeable clients */
					tv.tv_sec = 0;
					tv.tv_usec = 10000; // wait a shake (10 ms)
					result = select(FD_SETSIZE, (fd_set *)0, &wfds, (fd_set *)0, &tv );
					if (result  == -1)
						{
						perror("\nError in write select\n");
						exit(-3); // ??? break instead and close queues
						}
					if (result == 0)
						{
						/* should never get here, since sio is always (?) writable */
						printf("No writeable clients for message:%s\n",midistring);
						}
					else
					{	
					/* at this point we have a writeable fd set as active */
					/* find which one then service it*/
					for (wfd = 0; wfd < FD_SETSIZE; wfd++)
						{
						if (FD_ISSET(wfd, &wfds))
							{
							if ( (wfd != server_sockfd)&&(wfd != fd) )
							  {
							  /* then it was a non sio client, write */
							 sprintf(response, "%s\n",midistring); //!!!!
						    if (write(wfd, response, strlen(response)+1) <= 0)
							     {
							     perror("\nError sending client message\n");
							     exit(-4);
							     }
							  }
							}
						}
					} //end of else					
					}
			  }
			else	//then it must be a client
			  {
			  ioctl(afd, FIONREAD, &retval);
			  if (retval == 0)
			    {
			    close(afd);
			    FD_CLR(afd, &readfds);
			    printf("removing client on fd %d\n",afd);
			    }
			  else  /* then it is a legit client read */
			    {
			    retval = read(afd, string, 79);
			    if (retval < 0)
			     {
			     printf("\nError %d, errno %d receiving message\n",retval,errno);
				  perror("\nError in client read");
			     /* exit(-3); */
			     }
             infd = afd;   /* keep track of who sent it */
				  /* first convert string */
				 strcvt(string);
				 sscanf(string,"%x %x %x",&response[0],&response[1],&response[2]);
				 response[3]='\0'; 
				 if (debug >= 1)
					{
					printf("Output to Midi %x %x %x\n",
						 response[0],response[1],response[2]);
					fflush(stdout);
					}
				 /* write to the serial device */
				 for (i=0; i<3; i++)
				 		{
				    retval = write(fd, &response[i], 1);
					 usleep(350);
					 	}
			    if (retval <= 0)
			     {
			     perror("\nError sending message to sio\n");
			     exit(-5);
			     }
             /* write to all the other clients */
             tv.tv_sec = 0;
				 tv.tv_usec = 10000; // wait a shake (10 ms)
				 result = select(FD_SETSIZE, (fd_set *)0, &wfds, (fd_set *)0, &tv );
				 if (result  == -1)
				   {
					perror("\nError in write select\n");
					exit(-3); // ??? break instead and close queues
					}
				 if (result == 0)
					{
					/* should never get here, since sio is always (?) writable */
					printf("No writeable clients for message:%s\n",midistring);
					}
				 else
					{	
					/* at this point we have a writeable fd set as active */
					/* find which one then service it*/
				   sprintf(midistring,"%02x %02x %02x\n",response[0],response[1],response[2]);
					for (wfd = 0; wfd < FD_SETSIZE; wfd++)
						{
						if (FD_ISSET(wfd, &wfds))
							{
							if ((wfd != server_sockfd)&&(wfd != fd)&&(wfd != infd))
							  {
							  /* then it was a non sio other client, write */
						     if (write(wfd, midistring, strlen(midistring)+1) <= 0)
							     {
							     perror("\nError sending client message\n");
							     exit(-4);
							     }
							  }
							}
						}
					} //end of else					
					
			    } /* end of legit client read */
			  }
			}
		}

	}	//end of while running

//cleanup
fprintf(stdout,"\n");
exit (0);

}	//end of main
/*******************************Functionsville******************************/
/***************************************************************/
/* midi_init()                                                 */
/* initializes the midi device, including reading the config   */
/* file MIDI.CFG                                               */
/* returns a 0 if ok, a value < 0 if an error occurred         */
/* 9/12/93: Modified filter for a depth of 6 notes             */
/* 10/25/96:Removed need for CFG file entry number             */
/* 1/2/98:  Converted to unix mididd 									*/
/***************************************************************/
int midi_init(void)
{
FILE *config_file;    /* configuration input file */
FILE *diag;    /* diagnostics file */
char string[80];
char in_string[80];
extern struct midi_struct midi;  /* midi device structure */
char *p;
int i;
int retval = 0;

/* first initialize the midi structure  */
midi.interp = -1;
midi.stat = 0;
midi.count = 0;
midi.note_g = 0;  /* default to all filter stuff off */
midi.vel_g = 0;
midi.pres_g = 0;
midi.width = 0;
midi.note_on_p = 0;
midi.note_off_p = 0;
midi.chan_p = 0;


/* now get the configuration file */
config_file = fopen("mididd.cfg","r");
if (config_file == NULL)
	{
   fclose(config_file);
   retval = -1;  /* file open error */
   }
else
{
/* if we are still here, we must read the file */
p = fgets(in_string,79,config_file);
i=0;
while (p != NULL)
   {
   sscanf(in_string, "%s", string);
   if (strcmp(string,"interpretation") == 0)
      sscanf(in_string,"%s %d",string,&midi.interp);
   if (strcmp(string,"note_gran") == 0)
      sscanf(in_string,"%s %d",string,&midi.note_g);
   if (strcmp(string,"velocity_gran") == 0)
      sscanf(in_string,"%s %d",string,&midi.vel_g);
   if (strcmp(string,"touch_gran") == 0)
      sscanf(in_string,"%s %d",string,&midi.pres_g);
   if (strcmp(string,"width") == 0)
      sscanf(in_string,"%s %d",string,&midi.width);
   p = fgets(in_string,79,config_file);
   }
}
       
/* now do value checking on a few things */
if (midi.width > 6)
   midi.width = 6;   /* max of three notes for the width */

/* write the diagnostics */
diag = fopen("mididd.diags","a");
fprintf(diag,"\n\nMidi Initialization Data:");
if (retval == -1)
	fprintf(diag,"\nUsing Default Values");
fprintf(diag,"\nMidi interpretation %d.",midi.interp);
fprintf(diag,"\nNote granularity %d.",midi.note_g);
fprintf(diag,"\nVelocity granularity %d.",midi.vel_g);
fprintf(diag,"\nAfter Touch granularity %d.",midi.pres_g);
fprintf(diag,"\nFilter width %d.",midi.width);
fclose(diag);

fclose(config_file);

return retval;
}

/***************************************************************/
/* midi_in(channel)                                            */
/* inputs from the midi device                                 */
/* returns the data in the in_buff                             */
/* returns a value to indicate status                          */
/* 0 = took a value and are processing it or had no input      */
/* -1 = ignored the byte                                       */
/* >0 = fully formed accepted command with that many bytes     */
/* -10 = rejected midi command due to granularity              */
/* -100 = serious error (bombed out of compares)               */
/* other negative error is a port error                        */
/* 4/8/91:  Only looks for a note on or a note off.            */
/*          Passes either one for testing in trans             */
/* 8/29/93: Replaced with improved midi parsing routine.       */
/*          Added looking for after touch (channel pressure)   */
/* 9/12/93: Modified filter for a depth of 6 notes             */
/* 4/8/94:  Will ignore all commands but note on, off and      */
/*          channel pressure                                   */
/* 1/2/98:  Converted to unix mididd 									*/
/***************************************************************/
int midi_in(in)
int in;
{
/* in is the char fed to the midi input routine */
extern struct midi_struct midi;  /* midi device structure */
int retval = 0;
int i;
int ntemp;
int vtemp;
int ptemp;

if ((in >= 0 ) && (in < 0x100))
   {
   /* then there was a character */
   /* first check value */
   if (in < 0x80)
      {
      /* then it is a data byte, process based on stat and count info */
      if ( midi.stat == 0 )   /* then we are ignoring until we see status */
         {
         midi.in_buff[0] = '\0';
         return -1;   /* exit with no action other than taking byte */
         }
      else if ( (midi.stat != 0) && (midi.count == 1) )
         {
         midi.in_buff[0] = midi.stat;   /* put command byte in */
         midi.in_buff[1] = in;
         midi.in_buff[2] = '\0';
         /* check type of status byte to know number of data bytes */
         if ( (midi.stat >= 0xc0) && (midi.stat <= 0xdf) )
            {
            /* then it is a prog change or channel presure with 1 data byte */
            midi.count = 1;
            retval = 2;   /* return a fully formed 2 byte message */
            }
         else
            {
            /* then it is a channel voice command, first byte */
            midi.count = 2;
            return 0;
            }
         }
      else if ( (midi.stat != 0) && (midi.count == 2) )
         {
         /* then it is a channel voice command, second byte */
         midi.in_buff[2] = in;
         midi.in_buff[3] = '\0';
         midi.count = 1;
         retval = 3;   /* fully formed 3 byte command */
         }
      }
   else if ( (in >= 0x80) && (in <= 0x9f) )
      {
      /* then it is a note on or off status byte */
      midi.stat = in;
      midi.count = 1;
      return 0;
      }
   else if ( (in >= 0xa0) && (in <=0xcf) )
      {
      /* then it is a non-implemented status byte */
      midi.stat = 0;
      midi.count = 0;
      return -1;   /* exit with no action other than taking byte */
      }
   else if ( (in >= 0xd0) && (in <= 0xdf) )
      {
      /* then it is a channel pressure status byte */
      midi.stat = in;
      midi.count = 1;
      return 0;
      }
   else if ( (in >= 0xe0) && (in <=0xef) )
      {
      /* then it is a non-implemented status byte */
      midi.stat = 0;
      midi.count = 0;
      return -1;   /* exit with no action other than taking byte */
      }
   else if ( (in >= 0xf0) && (in <= 0xf7) )
      {
      /* then it is a system common message */
      midi.stat = 0;
      midi.count = 0;
      return -1;   /* exit with no action other than taking byte */
                  /* ignore all for now */
      }
   else if ( (in >= 0xf8) && (in <=0xff) )
      {
      /* then it is a system real time */
      //midi.in_buff[0] = in;
      //midi.in_buff[1] = '\0';
      //midi.in_buff[2] = '\0';
      //return 1;   /* exit with no action other than taking byte */
                  /* return = 1 means command of 1 byte */
      midi.stat = 0;
      midi.count = 0;
      return -1;   /* exit with no action other than taking byte */
                  /* ignore all for now */
      }
   else
      return -100;  /* very bad error */

   /* if we reach here, we have a command to return, now do filtering */
   if (midi.width == 0)
      return retval; /* if no width, shut filtering off */
   else if ( (midi.in_buff[0]&0xf0) == 0x90 )   /* if it is a note on */
      {
      /* then we have a note on to check*/
      for (i=0; i<midi.width; i++)
         {
         if (midi.note_g != 0)
            ntemp = midi.in_buff[1]-midi.note_on[i][1];
         else
            ntemp = 0;  /* be sure to pass compare */
         if (ntemp < 0) /* work in positive numbers */
            ntemp *= -1;

         if (midi.vel_g != 0)
            vtemp = midi.in_buff[2]-midi.note_on[i][2];
         else
            vtemp = 0;  /* be sure to pass compare */
         if (vtemp < 0)
            vtemp *= -1;

         if ( (midi.in_buff[0] == midi.note_on[i][0]) &&  /* same cmnd and chan */
               (ntemp <= midi.note_g) && (vtemp <= midi.vel_g) )
            return -10; /* rejected due to granularity */
         }
      /* if still here, we have a new note to keep */
      midi.note_on[midi.note_on_p][0] = midi.in_buff[0];
      midi.note_on[midi.note_on_p][1] = midi.in_buff[1];
      midi.note_on[midi.note_on_p][2] = midi.in_buff[2];
      midi.note_on_p++;
      if (midi.note_on_p >= midi.width)
         midi.note_on_p = 0;
      return retval;
      }
   else if ( (midi.in_buff[0]&0xf0) == 0x80 )   /* if it is a note off */
      {
      /* then we have a note off to check*/
      for (i=0; i<midi.width; i++)
         {
         if (midi.note_g != 0)
            ntemp = midi.in_buff[1]-midi.note_off[i][1];
         else
            ntemp = 0;  /* be sure to pass compare */
         if (ntemp < 0) /* work in positive numbers */
            ntemp *= -1;

         if (midi.vel_g != 0)
            vtemp = midi.in_buff[2]-midi.note_off[i][2];
         else
            vtemp = 0;  /* be sure to pass compare */
         if (vtemp < 0)
            vtemp *= -1;

         if ( (midi.in_buff[0] == midi.note_off[i][0]) &&  /* same cmnd and chan */
               (ntemp <= midi.note_g) && (vtemp <= midi.vel_g) )
            return -10; /* rejected due to granularity */
         }
      /* if still here, we have a new note off to keep */
      midi.note_off[midi.note_off_p][0] = midi.in_buff[0];
      midi.note_off[midi.note_off_p][1] = midi.in_buff[1];
      midi.note_off[midi.note_off_p][2] = midi.in_buff[2];
      midi.note_off_p++;
      if (midi.note_off_p >= midi.width)
         midi.note_off_p = 0;
      return retval;
      }
   else if ( ((midi.in_buff[0]&0xf0) == 0xd0) && (midi.pres_g != 0) )
      /* if it is a channel pressure with a non zero granularity */
      {
      /* then we have an aftertouch to check */
      for (i=0; i<midi.width; i++)
         {
         ptemp = midi.in_buff[1] - midi.chan_press[i][1];
         if (ptemp < 0)
            ptemp *= -1;

         if ( (midi.in_buff[0] == midi.chan_press[i][0]) &&  /* same cmnd and chan */
               (ptemp <= midi.pres_g) )
            return -10; /* rejected due to granularity */
         }
      /* if still here, we have a new note to keep */
      midi.chan_press[midi.chan_p][0] = midi.in_buff[0];
      midi.chan_press[midi.chan_p][1] = midi.in_buff[1];
      midi.chan_p++;
      if (midi.chan_p >= midi.width)
         midi.chan_p = 0;
      return retval;
      }
   else
      return retval;
   }
else if (in == 0x100)
   retval = 0;
else    /* must have had an error */
   retval = in;

return retval;
}

/************************************************/
/*      strcvt(char *c)                         */
/* Searches the string pointed to in the call   */
/* for any \ sequences and performes the        */
/* translation to the ascii code.  Copies the   */
/* translated string into the pointed to string.*/
/* Implements the following sequences           */
/* \n (0x0a), \r (0x0d), \t (0x09), \\ (0x5c)   */
/* \0 (0x00), \xx where xx is any ascii 00-ff   */
/* 1/2/98:  Converted to unix 		 				*/
/************************************************/
void strcvt(char *c)
{
int i = 0;
int j = 0;
int k;
int ival;
char s[3];  /* short string */
char outstr[128];  /* output string */
extern unsigned int device_diags;

if (1==2)
{
printf("\nString input:");
for (k=0; k<128; k++)
   if (c[k] == '\0')
      break;
   else
      printf("%2x ",c[k]);
}

while (c[i] != '\0')
   {
   if (c[i] == '\\')
      {
      /* then it is a start of explicit */
      s[0] = c[++i];
      s[1] = c[i+1];
      s[2] = '\0';   /* make a short string for conversion */
      switch   (s[0])
         {
         case  'n':  outstr[j++] = '\n';
                     break;
         case  'r':  outstr[j++] = '\r';
                     break;
         case  't':  outstr[j++] = '\t';
                     break;
         case  '\\': outstr[j++] = '\\';
                     break;
         case  '0':  
            {
            if( (s[1]<'0') || ((s[1]>'9')&&(s[1]<'A')) || 
                ((s[1]>'F')&&(s[1]<'a')) || (s[1]>'f') )
               outstr[j++] = '\0';
            else  /* then it is a hex code starting with a 0 */
               {
					ival = (int)strtol(s,(char **)NULL,16);
               //stch_i(s,&ival);
               outstr[j++] = (char)ival;
               i++;
               }
            break;
            }
         default:
							ival = (int)strtol(s,(char **)NULL,16);
							//stch_i(s,&ival);
                     outstr[j++] = (char)ival;
                     i++;
         }
      }
   else
      outstr[j++] = c[i];
   i++;
   }
outstr[j] = '\0';

if (1 == 2)
{
printf("\nString converted to:");
for (k=0; k<128; k++)
   if (outstr[k] == '\0')
      break;
   else
      printf("%2x ",outstr[k]);
}

strcpy(c,outstr);
return;
}


/************************************************************/
/* Thats it !                                               */
/************************************************************/
