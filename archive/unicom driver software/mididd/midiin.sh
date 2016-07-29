#!/bin/bash
# first test of a midi-in actor
# accepting midi notes from a telnet client
actor1 -d0 -n midiin -h 192.168.2.1 -p 9800 << CONFIG
192.168.2.1 9900 midi
   note_on
   midi_channel 0 
   low   0x20 
   hi    0x70 
   low_velocity   0x10 
   hi_velocity    0x70 
   end

# this is a comment
shell echo got midi note

shell sleep 10

192.168.2.1 9900 :90 40 40
 


CONFIG
