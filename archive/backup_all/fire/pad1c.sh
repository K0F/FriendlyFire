#!/bin/sh#actor -d1 -n pad1c -h 10.5.1.15 -p 9850 -f pad1.log << CONFIGactor -d0 -n pad1c -h 10.5.1.15 -p 9850 << CONFIG10.5.1.15 9800 midi   note_on   midi_channel 9    low   0x23   hi    0x23   low_velocity   0x2c   hi_velocity    0x3f   endif %video_mode -eq 1	{	10.5.1.15 9900 :8131 se pl\r	}    CONFIG