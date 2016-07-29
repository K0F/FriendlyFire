/*******************************************************************************
        File: MINIEXT.H           Ver 2.32                  December 29, 1996

         External variables, definitions, and initialization.

         Copyright (c) 1988 - 1994, Russ Gritzo, Jericho Data Systems.  
            No portion of this code may be used without author's consent.


        Written by:
                        Russ Gritzo
                        77 Mesa Verde
                        Los Alamos, NM, 87544
                        505-672-9229

        Compiled under Lattice C, Version 6.0


                Requires:        AT type computer
                                 Midi Portman connected to COM1
                                 Laser Disk connected to COM5
                                 Lights connected to COM6
                                 Sound connected to COM7

                Revision notes:  (Note: also change start up message)
                Ver 1.0     9/20/92      Original Version
                Ver 2.0     4/8/94       Reworked Version
                Ver 2.1     4/15/94      Added lights and sound
                Ver 2.11    4/22/94      256 midi keys
                Ver 2.12    10/1/94      Added timer on sysdev
                Ver 2.2     12/28/95     Added Com ports 8,9 and 10
                Ver 2.21    1/1/96       Added opto22 PAMUX board,
                                          512 midi keys, stop at max keys
                Ver 2.22    1/7/96       Added stepper device as an input
                Ver 2.3     10/15/96     Added sensor device as an input,
                                          removed slide device
                Ver 2.31    10/25/96     Added system device as input
                Ver 2.32    12/29/96      Added entry to device structure for 
                                             pacing and delay
                                          Changed to bit mapped diagnostics

*******************************************************************************/

unsigned int device_diags; /*bit mapped by device number */
unsigned int port_diags;   /*bit mapped by port number */


/* midi device */
//struct midi_key      {
//   int   lo_note;    /* range low note (inclusive) */
//   int   hi_note;    /* range hi note (inclusive)  */
//   int   lo_vel;     /* range low velocity (inclusive) */
//   int   hi_vel;     /* range hi velocity (inclusive) */
//   int   mchan;      /* midi channel number */
//   int   mcmd;       /* midi note on or off */
//   char  mlabel[20]; /* system label */
//   int   mvalue;     /* system label value */
//   int   mdev;       /* midi message destination device */
//   char  mstring[80];   /* midi message to device */
//                     };    /* key table nested structure */

struct midi_struct   {
   int   interp;     /* interpretation flag */
   int  stat;     /* status buffer */
   int   count;   /* counter */
   int   note_g;
   int   vel_g;
   int   pres_g;                                                   
   int   width;
   int   note_on_p;
   int   note_off_p;
   int   chan_p;
   int   max_keys;
   int  note_on[6][3];  /*6 notes with channel number, note and velocity */
   int  note_off[6][3];
   int  chan_press[6][2];
	char  in_buff[10];	// replaces the inbuffer
	char	out_buff[10];  //replaces the outbuffer
//   struct midi_key key[512];   /* midi key table */
                     };
struct midi_struct midi;  /* midi device structure */


