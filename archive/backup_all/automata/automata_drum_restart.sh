#!/bin/shactor132 -d0 -n automata_drum_restart -h 10.5.1.12 -p 9850  << CONFIG10.5.1.12 9800 midi   note_on   midi_channel 15    low   0x10   hi    0x7f   low_velocity   0x01   hi_velocity    0x7f    end# three10.5.1.12 9800 :90 3e 7fshell msleep 50010.5.1.12 9800 :80 3e 7f# two10.5.1.12 9800 :90 3d 7fshell msleep 50010.5.1.12 9800 :80 3d 7f	# one10.5.1.12 9800 :90 3c 7fshell msleep 50010.5.1.12 9800 :80 3c 7f10.5.1.12 9900 :\0218 will be restarting soon\r#shell /actors/automata_restart.sh &CONFIG