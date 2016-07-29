/*****************************************************************************

   File: ACTOR1.C              Version 1.05                 Date: 2/16/98   

   This is the actor for UNICOM

	This is a CLIENT, Talking to multiple servers
   
      Intended for use with the following:
      		unix (linux) machine using unicom device drivers,
            including logicdd Version 1.0

		Compile with:
				gcc actor1.c -o actor1 -lgdbm
            
      The revision history is shown below:

            Rev 0.1 1/25/98  Initial version
            Rev 0.2 2/1/98   Revised to remain persistant
            Rev 1.0 2/11/98  Significant rework for:
                              Logic (if and orif), multiple commands and
                              Command groups.
            Rev 1.01 2/12/98 Changed location of db open for string retrieval
                              to try and fix dp opening errors.
            Rev 1.02 2/13/98 Added 0x0d to the output strings.
            Rev 1.03 2/15/98 Changed 0x0d to \n.
				Rev 1.04 2/15/98 Changed midi input parsing to reflect \n at end of midi
										output string, consistent with mididd 1.4
            Rev 1.05 2/16/98 Changed databse file to be actor name, default actor
                              Added immediate input type
				
  
      The following functions are in place, with the indicated
   last time of modification:
		
   calling flags:
      -v Set verbose flag to 1, equiv to -d 1
      -f <filename> output to <filename> for logging
      -h <host_addr> Host address of logicdd, must also have -p
      -p <host_port> Port number of logicdd, must also have -h
      -n <name> Actor name and name of database file
      -l <life> Startup life value
      -d <0|1|2> Debug modes, -d 0 eqiv to no verbose, -d 1 equiv to verbose
               
   outputs information to stdout, errors to stderr

   Note:    input strings limited to 80 chars total
            host port command statement must be of the format:
             127.0.0.1 9999 : command string
            If no logicdd, assumes all conditions are true.
   
   TODO:    improve string processing on shell and host/port/command 
            strcvrt (poss?) on shell and commands
            catch signals
            get logicdd port assignment from /etc/services
            remove database under running, use malloc'd structure
            make list of sockets, keep open

	  
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <gdbm.h>

#define OPTIONS "vf:h:p:n:l:d:"
#define VERSION "Version 1.05, 2/16/98"

/* prototypes */
int logitran(char string[]);
int parse(void);
int cmd_out(int k);
void msg(char *outstring);

/* Define midi key structure */
struct midi_key      {
   int   lo_note;    /* range low note (inclusive) */
   int   hi_note;    /* range hi note (inclusive)  */
   int   lo_vel;     /* range low velocity (inclusive) */
   int   hi_vel;     /* range hi velocity (inclusive) */
   int   mchan;      /* midi channel number */
   int   mcmd;       /* midi note on or off */
   int   incmd;      /* midi input command */
   int   innote;     /* midi input note value*/
   int   invel;      /* midi input in vel */
                     };    /* key table nested structure */
struct midi_key key;

/* Define the immediate structure */
struct immediate  {
   int   initdel; /* delay until the first pass thru */
   int   loopdel; /* delay between successive loops */
   int   iter;    /* number of iterations to perform , 0 = forever */
   int   n;       /* counter for the number of loops */
                  };
struct immediate imm;
  

FILE *logf;
int verbose = 0;
int life = 0; /* immortal */
char msgstr[120];
char name[40]; /* actor name */
char dbfile[40];  /* database file */
char datstring[20]; /* database variables */
char keystring[20];
int logicfd;   /* logicdd socket file handle */
GDBM_FILE db;
datum dbkey;
datum dbdata;

main(int argc, char *argv[])
{
extern int logicfd;   /* logicdd socket file handle */
extern char dbfile[];  /* database file */
extern GDBM_FILE db;
int opt;
char outfile[40];
char inhost[40];
int inport;
char lhost[40];
int lport;
int intype; /* 0 = midi input, 1 = discrete input */
unsigned char ch;
char *p;
int i,n;
int retval;
/* socket variables */
int insockfd;
struct sockaddr_in address;
int len;
/* string variables */
char string[120];
char inchar;
char in_string[120];
int max_outs;

/* init any variables needed */
strcpy(name,"actor");   /* default name */
outfile[0]='\0';
key.lo_note = -1;
key.hi_note = 0x7f;   /* note max, will pass comparison */
key.lo_vel = -1;
key.hi_vel = 0x7f;
key.mchan=-1;
key.mcmd=0;
imm.n = 0;
imm.iter = 0;
verbose = 1;
logf=NULL;

sprintf(msgstr,"Starting actor version %s\n",VERSION);
msg(msgstr);
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
         strncpy(outfile,optarg,39);
         outfile[39]='\0'; /* prevent buffer overruns */
         sprintf(msgstr,"Output to file %s ",outfile);
         msg(msgstr);
         break;
      case 'h':
         strncpy(string,optarg,39);
         string[39]='\0'; /* prevent buffer overruns */
         sscanf(string,"%s",lhost);
         sprintf(msgstr,"Logicdd host is %s ",lhost);
         msg(msgstr);
         break;
      case 'p':
         strncpy(string,optarg,39);
         string[39]='\0'; /* prevent buffer overruns */
         sscanf(string,"%d",&lport);
         sprintf(msgstr,"Logicdd port is %d ",lport);
         msg(msgstr);
         break;
      case 'n':
         strcpy(name,optarg);
         name[39]='\0'; /* prevent buffer overruns */
         sprintf(msgstr,"Name: %s ",name);
         msg(msgstr);
         break;
      case 'l':
         sscanf(optarg,"%d",&life);
         sprintf(msgstr,"Lifetime: %d ",life);
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

/* if a name specified, make a unique name for the database */
sprintf(dbfile,"./%s.db",name);

/* open the output logging file if any */
if (strlen(outfile)>0)
   {
   logf = fopen(outfile,"w");
   if (logf == NULL)
      {
      perror("Unable to open output file");
      exit(-1);
      }
   }

/* open up the logic device driver socket connection */
logicfd = socket(AF_INET, SOCK_STREAM, 0);
address.sin_family = AF_INET;
address.sin_addr.s_addr = inet_addr(lhost);
address.sin_port = htons(lport);
len = sizeof(address);
retval = connect(logicfd, (struct sockaddr *)&address, len);
if (retval == -1)
   {
   sprintf(msgstr,
      "Unable to connect to logicdd on %s:%d",lhost,lport);
   perror(msgstr);
   msg(msgstr);
   logicfd = -1;   /* tell the rest of the code that no logic */
   /* note: assume all conditions are TRUE */
   }

/* now start processing the configuration file */
/* first read in the input host section */
p=fgets(in_string,79,stdin);
if (p == NULL)
   {
   msg("Unexpected EOF in input host section of config file\n");
   exit(-3);   /* unexpected EOF in a key table entry */
   }
sscanf(in_string,"%s %d %s", inhost, &inport, string);
if (strcasecmp(string,"midi") == 0)
   {
   intype = 0;
   /* look for midi input type data */
   while(1==1)
      {
      p=fgets(in_string,79,stdin);
      if (p == NULL)
         {
         msg("Unexpected EOF in input section of config file\n");
         exit(-3);   /* unexpected EOF in a key table entry */
         }
      
      sscanf(in_string, " %s ", string);
      /* now look for the various entries */
      if (strcasecmp(string,"low") == 0)
         sscanf(in_string," %s %x ",string,&key.lo_note);
      else if (strcasecmp(string,"hi") == 0)
         sscanf(in_string," %s %x ",string,&key.hi_note);
      else if (strcasecmp(string,"low_velocity") == 0)
         sscanf(in_string," %s %x ",string,&key.lo_vel);
      else if (strcasecmp(string,"hi_velocity") == 0)
         sscanf(in_string," %s %x ",string,&key.hi_vel);
      else if (strcasecmp(string,"midi_channel") == 0)
         sscanf(in_string," %s %d ",string,&key.mchan);
      else if (strcasecmp(string,"note_on") == 0)
         key.mcmd = 0x90;
      else if (strcasecmp(string,"note_off") == 0)
         key.mcmd = 0x80;
      else if (strcasecmp(string,"after_touch") == 0)
         key.mcmd = 0xd0;
      else if (strcasecmp(string,"end") == 0)
         break;
      }
   }
else if (strcmp(string,"discrete") == 0)
   {
   intype = 1;
   /* look for discrete input type data */
   }
else if (strcmp(string,"immediate") == 0)
   {
   intype = 2;
   /* look for the immediate input type data */
   while(1==1)
      {
      p=fgets(in_string,79,stdin);
      if (p == NULL)
         {
         msg("Unexpected EOF in input section of config file\n");
         exit(-3);   /* unexpected EOF in a key table entry */
         }
      sscanf(in_string, " %s ", string);
      /* now look for the various entries */
      if (strcasecmp(string,"init_delay") == 0)
         sscanf(in_string," %s %d ",string,&imm.initdel);
      else if (strcasecmp(string,"loop_delay") == 0)
         sscanf(in_string," %s %d ",string,&imm.loopdel);
      else if (strcasecmp(string,"iterations") == 0)
         sscanf(in_string," %s %d ",string,&imm.iter);
      else if (strcasecmp(string,"end") == 0)
         break;
      }
   }
else
   {
   sprintf(msgstr,"error - undefined input type %s",string);
   msg(msgstr);
   exit (-2);
   }
sprintf(msgstr,"Connection to type %d input on %s:%d",intype,inhost,inport);
msg(msgstr);

if ((verbose) && (intype==0))
   {
   /* print the midi key table for debugging */
   sprintf(msgstr,"\nMidi note low = %x",key.lo_note);
   msg(msgstr);
   sprintf(msgstr,"\nMidi note hi = %x",key.hi_note);
   msg(msgstr);
   sprintf(msgstr,"\nMidi velocity low = %x",key.lo_vel);
   msg(msgstr);
   sprintf(msgstr,"\nMidi velocity hi = %x",key.hi_vel);
   msg(msgstr);
   if (key.mcmd == 0x90)
      sprintf(msgstr,"\nNote ON, Midi channel %d.",key.mchan);
   else if (key.mcmd == 0x80)
      sprintf(msgstr,"\nNote OFF, Midi channel %d.",key.mchan);
   else if (key.mcmd == 0xd0)
      sprintf(msgstr,"\nAfter Touch, Midi channel %d.",key.mchan);
   else
      sprintf(msgstr,"\nNote ON_OFF ERROR, Midi channel %d.",key.mchan);
   msg(msgstr);
   msg("\n");
   }
else if ((verbose) && (intype==1))
   {
   /* print the discrete input table for debugging */
   }
else if ((verbose) && (intype==2))
   {
   /* print the immediate table for debugging */
   sprintf(msgstr,"\nInitial delay of %d seconds ",imm.initdel);
   msg(msgstr);
   sprintf(msgstr,"\nLoop delay of %d seconds ",imm.loopdel);
   msg(msgstr);
   sprintf(msgstr,"\nNumber of iterations %d ",imm.iter);
   msg(msgstr);
   }

/* process the rest of the inputs and place them into storage */
max_outs = parse();
if (max_outs < 0)
   {
   sprintf(msgstr,"parsing error %d\n",max_outs);
   msg(msgstr);
   }
sprintf(msgstr,"parsing returned %d commands\n",max_outs);

if (intype == 0)
   {
   /* now open the input socket and listen carefully */
   insockfd = socket(AF_INET, SOCK_STREAM, 0);
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr(inhost);
   address.sin_port = htons(inport);
   len = sizeof(address);

   retval = connect(insockfd, (struct sockaddr *)&address, len);
   if (retval == -1)
      {
      sprintf(msgstr,
         "Unable to connect to input on %s:%d",inhost,inport);
      perror(msgstr);
      msg(msgstr);
      exit(-5);
      }
   }
   
/* now open the database for reading the strings */
db = gdbm_open(dbfile, 512, GDBM_READER, 0x666, 0);
if (db == NULL)
   {
   sprintf(msgstr,"Failed to open database file for reading %s\n",dbfile);
   msg(msgstr);
   return(-1);
   }

/* if an immediate, then sleep the initdelay */
if ((intype == 2) && (imm.initdel > 0))
   sleep(imm.initdel);
   
/* now process the inputs */   
while(life >= 0)
   {
   if (intype == 0)
      {
   	n=0;
      while (n<100)
         {
         retval = read(insockfd, &inchar, 1);
         if (retval <= 0)
            {
            perror("Read error on input socket");
            msg("Read error on input socket\n");
            exit(-6);
            }
   		else if (inchar == '\0'); /* ignore a end of string null */
         else if ((inchar == '\n')||(inchar == 0x0d)||(inchar == 0x0a))
            {
            in_string[n] = '\0';
            break;
            }
   		else
   			in_string[n++]=inchar;
         }
      /* here we should have a midi note */
      sscanf(in_string,"%x %x %x", &key.incmd, &key.innote, &key.invel);
      if ((key.innote >= key.lo_note) && (key.innote <= key.hi_note) &&
          (key.invel >= key.lo_vel) && (key.invel <= key.hi_vel) &&
          (key.incmd == (key.mcmd+key.mchan)) )
         { /* whew - then we actually have an input, go do the output */

         if (verbose)
            {
            sprintf(msgstr,"\nMatch on midi in %x %x %x ",key.incmd,key.innote,key.invel);
            msg(msgstr);
            }
         /* now go do the outputs */
         for (i=1; i<=max_outs; i++)
            if (cmd_out(i) < 0)
               life = -1;
         }
      }
   else if (intype == 2)
      {
      /* just go do the outputs */
      for (i=1; i<=max_outs; i++)
         if (cmd_out(i) < 0)
            life = -1;

      imm.n++;
      if ((imm.iter > 0)&&(imm.n >= imm.iter))
         {
         life = -1;
         break;
         }

      /* now sleep the loopdelay */
      if ((intype == 2) && (imm.loopdel > 0))
         sleep(imm.loopdel);
      }

   } /* end of life for this actor */

gdbm_close(db);
close(insockfd);
msg("Actor terminated\n");
fclose (logf);

}

/********************************Functionsville ***********************/
/**********************************************************************
*
* Message output routine
* Takes input string, and outputs it to screen (if verbose)
* and to logging file, if provided
*
**********************************************************************/
void msg(char *outstring)
{
extern FILE *logf;
extern int verbose;
extern char msgstr[];
int i=0;

if (verbose)
   {
   printf("%s",outstring);
   }
if (logf != NULL)
   fprintf(logf,"%s",outstring);
if (logf != NULL)
   fflush(logf);
}

/**********************************************************************
*
* Command output routine
* Retrieve a command string from the database and output it
*
**********************************************************************/
int cmd_out(int k)
{
extern char name[]; /* actor name */
extern char datstring[];
extern char keystring[];
extern GDBM_FILE db;
extern datum dbkey;
extern datum dbdata;
extern int verbose;
extern char msgstr[];
extern int logicfd;   /* logicdd socket file handle */
char string[80];
char cmd[40];
char var[40];
char val[40];
int port = 0;
int shell = 0;
/* socket variables */
int outsockfd;
struct sockaddr_in address;
int len;
int retval;
int i, j;


/* look for the ifkey */
sprintf(keystring,"ifkey%d",k);
dbkey.dptr = (void *)keystring;
dbkey.dsize = strlen(keystring);
dbdata = gdbm_fetch(db, dbkey);
memcpy(string,dbdata.dptr,dbdata.dsize);
string[dbdata.dsize]='\0';
free(dbdata.dptr);
if (dbdata.dptr != NULL)
   {
   /* then it is conditional */
   if (logicfd != -1)
      retval = logitran(string); /* translate any %delimited variables */
   else
      retval = 0; /* treat as if it were all true */
   if (retval == 0); /* yippee, we are good to go */
   else if (retval == 256) /* command went, but was not true */
      return (0);
   else if (retval == 128) /* error in logicdd, assume cmd false */
      return (0);
   else if (retval == 127) /* unable to start shell */
      {
      sprintf(msgstr,"unable to start shell, system test error %d\n",retval);
      msg(msgstr);
      return (-1);
      }
   else  /* something else went wrong */
      {
      sprintf(msgstr,"system test error %d\n",retval);
      msg(msgstr);
      return (-1);
      }
   /* if condition is met, proceed, if not return */
   }

/* then it is unconditional, either way, get the command string*/
sprintf(keystring,"cmdkey%d",k);
dbkey.dptr = (void *)keystring;
dbkey.dsize = strlen(keystring);
dbdata = gdbm_fetch(db, dbkey);
memcpy(string,dbdata.dptr,dbdata.dsize);
string[dbdata.dsize]='\0';
free(dbdata.dptr);
if (dbdata.dptr == NULL)
   {
   /* this should never happen, if it does it is bad */
   sprintf(msgstr,"cmdkey %d retrieved null data\n",k);
   msg(msgstr);
   return (-1);
   }
/* now look at the string and figure what to do */
sscanf(string,"%s",cmd);
if (strcasecmp(cmd,"shell") == 0)   /* then it is a shell command */
   {
   for (i=6; i<(strlen(string)+6); i++)
      string[i-6]=string[i];
   msg("sending system: ");
   msg(string);
   msg("\n");
   system(string);
   }
else if (strcasecmp(cmd,"life") == 0)   /* then it is a life decision */
   {
   sscanf(string,"%s %d",cmd,&life);
   sprintf(msgstr,"Setting life to %d\n",life);
   msg(msgstr);
   }
else if (strcasecmp(cmd,"set") == 0)   /* then it is a set variable */
   {
   sscanf(string,"%s %s %s",cmd,var,val);
   sprintf(msgstr,"Setting variable %s to %s\n",var,val);
   msg(msgstr);
   sprintf(string,"%s %s",var,val);
   retval = write(logicfd, string, strlen(string));
   if (retval == -1)
      {
      sprintf(msgstr,"Unable to write to logicdd in set\n");
      msg(msgstr);
      }
   retval = read(logicfd, string, 79);
   if (retval < 0)
      {
     sprintf(msgstr,"Error %d reading from logicdd in set\n",retval);
     msg(msgstr);
      }
   }
else  /* assume it is a host port command message */
   {
   sscanf(string,"%s %d",var,&port);
   /* try and cleanup the string */
   for (i=0; i<strlen(string); i++)
      if (string[i]==':')
         break;
   i++; /* get past the colon */
   for (j=i; j<(strlen(string)+i); j++)
      string[j-i] = string[j];
   /* then open the server socket and do */
   outsockfd = socket(AF_INET, SOCK_STREAM, 0);
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr(var);
   address.sin_port = htons(port);
   len = sizeof(address);

   retval = connect(outsockfd, (struct sockaddr *)&address, len);
   if (retval == -1)
      {
      sprintf(msgstr,
       "Unable to connect to output on %s:%d \n",var,port);
      msg(msgstr);
      }
   else
      {
      msg("sending socket: ");
      msg(string);
      retval = write(outsockfd, string, strlen(string));
      if (retval == -1)
         {
         sprintf(msgstr,
          "Unable to write to output on %s:%d ",var,port);
         msg(msgstr);
         }
      msg("\n");
      }
   close(outsockfd);
   }
return(1);
}

/**********************************************************************
*
* Input parsing routine
* Retrieve lines of input from stdin and build command /logic strings
*
**********************************************************************/
int parse(void)
{
extern char dbfile[];  /* database file */
extern char name[]; /* actor name */
extern char datstring[];
extern char keystring[];
extern GDBM_FILE db;
extern datum dbkey;
extern datum dbdata;
extern int verbose;
extern char msgstr[];
int level = 0;
int multi = 0;
int i, n, j, k;
int retval;
char tstring[5][80]; /* support 4 levels */
char instring[80];
char cmd[40];
char string[80];
char ifstr[20];
char varstr[20];
char oprstr[20];
char valstr[20];

string[0] = '\0';
for (i=0; i<5; i++)
   tstring[i][0] = '\0';


db = gdbm_open(dbfile, 512, GDBM_NEWDB, 0x666, 0);
if (db == NULL)
   {
   sprintf(msgstr,"Failed to open database file %s\n",dbfile);
   msg(msgstr);
   return(-1);
   }

j = 0;
k = 0;
   
/* now process the input from stdin */
while (1==1)
{
j++;
if (gets(instring) == NULL)
   break;

/* no clue why this was in here, but i left it */
for (i=0; i<strlen(instring); i++)
   if (instring[i] == '\n')
      instring[i] = ' ';

n=sscanf(instring," %s ",cmd);
if (n < 1)
   continue;
/* clear out leading white spaces */
for (i=0; i<strlen(instring); i++)
   if ((instring[i] != ' ')&&(instring[i] != '\t'))
      break;
n=0;
while(i<strlen(instring))
   instring[n++]=instring[i++];
instring[n]='\n';
instring[n+1]='\0';

if (strcmp(cmd,"}") == 0)
   {
   /* close multi */
   if ((multi <= 0)||(level <= 0))
      {
      sprintf(msgstr,"Syntax error, unexpected } in line %d: %s\n",j,instring);
      msg(msgstr);
      return(-2);
      }
   multi--;
   level--;
   }
else if (strcmp(cmd,"{") == 0)
   {
   /* open multi */
   multi++;
   }
else if (strcmp(cmd,"if") == 0)
   {
   /* open if level */
   level++;
   if (level >= 5)
      {
      sprintf(msgstr,"Syntax error, exceeded 4 level if in line %d: %s\n",j,instring);
      msg(msgstr);
      return(-2);
      }
   n=sscanf(instring,"%s %s %s %s",ifstr,varstr,oprstr,valstr);
   if (n < 4)
      {
      sprintf(msgstr,"Syntax error, %d args, expecting 4 in line %d: %s\n",n,j,instring);
      msg(msgstr);
      return(-2);
      }
   if (level > 1)
      sprintf(tstring[level]," -a %s %s %s",varstr,oprstr,valstr);
   else
      sprintf(tstring[level],"%s %s %s",varstr,oprstr,valstr);
   }
else if (strcmp(cmd,"orif") == 0)
   {
   /* open if level */
   if (level == 0)
      {
      sprintf(msgstr,"Syntax error, orif without if in line %d: %s\n",j,instring);
      msg(msgstr);
      return(-2);
      }
   level++;
   if (level >= 5)
      {
      sprintf(msgstr,"Syntax error, exceeded 4 level if in line %d: %s\n",j,instring);
      msg(msgstr);
      return(-2);
      }
   n=sscanf(instring,"%s %s %s %s",ifstr,varstr,oprstr,valstr);
   if (n < 4)
      {
      sprintf(msgstr,"Syntax error, %d args, expecting 4 in line %d: %s\n",n,j,instring);
      msg(msgstr);
      return(-2);
      }
   if (level > 1)
      sprintf(tstring[level]," -o %s %s %s",varstr,oprstr,valstr);
   else
      sprintf(tstring[level],"%s %s %s",varstr,oprstr,valstr);
   }
else if (instring[0] == '#');
else
   {
   /* assume hpc, sc, lv, svv */
   string[0]='\0';
   k++;  /* use as the key index */
   if (level > 0)
      {
      strcat(string,"[ ");
      strcat(string,tstring[1]);
      for (i=2; i<=level; i++)
         {
         /* strcat(string,"&"); */
         strcat(string,tstring[i]);
         }
      strcat(string," ] ");
      /* add an if key and string to the db */
      sprintf(keystring,"ifkey%d",k);
      dbkey.dptr = (void *)keystring;
      dbkey.dsize = strlen(keystring);
      dbdata.dptr = (void *)string;
      dbdata.dsize = strlen(string);
      retval = gdbm_store(db, dbkey, dbdata, GDBM_REPLACE);
      if (retval != 0)
         {
         sprintf(msgstr,"Error updating db for key %d data %s\n",k,datstring);      
         msg(msgstr);
         }
      }
   /* now store the input string */
   sprintf(keystring,"cmdkey%d",k);
   dbkey.dptr = (void *)keystring;
   dbkey.dsize = strlen(keystring);
   dbdata.dptr = (void *)instring;
   dbdata.dsize = strlen(instring);
   retval = gdbm_store(db, dbkey, dbdata, GDBM_REPLACE);
   if (retval != 0)
      {
      sprintf(msgstr,"Error updating db for key %d data %s\n",k,datstring);      
      msg(msgstr);
      }
   if (level > multi)
      level--;
   }
} /* end of while */

if (verbose)
{
msg("Stored the commands:\n");
for (i=1; i<=k; i++)
   {
   sprintf(keystring,"ifkey%d",i);
   dbkey.dptr = (void *)keystring;
   dbkey.dsize = strlen(keystring);
   dbdata = gdbm_fetch(db, dbkey);
   memcpy(string,dbdata.dptr,dbdata.dsize);
   string[dbdata.dsize]='\0';
   free(dbdata.dptr);
   if (dbdata.dptr != NULL)
      {
      sprintf(msgstr,"ifkey %d retrieved data %s\n",i,string);
      msg(msgstr);
      }
   sprintf(keystring,"cmdkey%d",i);
   dbkey.dptr = (void *)keystring;
   dbkey.dsize = strlen(keystring);
   dbdata = gdbm_fetch(db, dbkey);
   memcpy(string,dbdata.dptr,dbdata.dsize);
   string[dbdata.dsize]='\0';
   free(dbdata.dptr);
   if (dbdata.dptr != NULL)
      {
      sprintf(msgstr,"cmdkey %d retrieved data %s\n",i,string);
      msg(msgstr);
      }
   }
}

gdbm_close(db);

return(k);
}

/**********************************************************************
*
* Logic evaluation routine
* Translate the logic string using the logicdd and send it to shell
* for evaluation
*
**********************************************************************/
int logitran(char string[])
{
extern int logicfd;   /* logicdd socket file handle */
extern struct midi_key key;
extern int verbose;

int retval;
int i, j, k;
int index, outdex;
char s2[120];
char vs[40];
char rs[80];
char keystring[40];
char datstring[40];

index = 0;
outdex = 0;

while(index <= strlen(string))
{
/* first go thru and find the % variables */
j=outdex;
for (i=index; i<=strlen(string); i++)
   {
   if ((string[i] == '%')||(string[i]=='\0'))
      break;
   s2[j++]=string[i];
   }
s2[j]='\0';

if ((i>strlen(string))||(string[i]=='\0'))
   break;

/* now remove the %variable upto the blankspace */
k=0;
i++;  /* move past the % */
while(i<=strlen(string))
   {
   if (string[i] == ' ')
      break;
   vs[k++]=string[i++];
   }
vs[k]='\0';
index = i;

/* first fish out the internal variables */
/* from key.incmd, key.innote, key.invel */
if (strcmp(vs,"mnote") == 0)
   sprintf(datstring,"%d",key.innote);
else if (strcmp(vs,"mchan") == 0)
   sprintf(datstring,"%d",(key.incmd & 0x0f));
else if (strcmp(vs,"mstat") == 0)
   sprintf(datstring,"%d",(key.incmd & 0xf0));
else if (strcmp(vs,"mvel") == 0)
   sprintf(datstring,"%d",key.invel);
else
   {
   /* assume you must do the db lookup */
   /* now translate the varstring by doing the database socket call */   
   retval = write(logicfd, vs, strlen(vs));
   if (retval == -1)
      {
      sprintf(msgstr,"Unable to access logicdd\n");
      msg(msgstr);
      return 0;   /* consider it to be true */
      }
   retval = read(logicfd, rs, 79);
   if (retval < 0)
     {
     sprintf(msgstr,"Error %d reading logicdd\n",retval);
     msg(msgstr);
     return 128;  /* flag as some other error */
     }
   rs[retval] = '\0';
   /* scan the string for the db response */ 
   retval = sscanf(rs,"%s %s",keystring,datstring);
   }
strcat(s2,datstring);
   
/* now pack the rest of the string */
outdex=strlen(s2);

}

/* now finally pass the string and get the results */
if (verbose >= 2)
   {
   sprintf(msgstr,"Testing:%s\n",s2);
   msg(msgstr);
   }
retval = system(s2);

if ((retval == 139)||(retval == 32512))
   {
   sprintf(msgstr,"systest error, returned %d testing:%s\n",retval,s2);
   msg(msgstr);
   }

return retval;
}

