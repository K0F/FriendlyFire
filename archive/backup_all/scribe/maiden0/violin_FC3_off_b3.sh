#!/bin/bashactor -d1 -n violin_FC3_off_b3 -h 192.168.2.14 -p 9850 << CONFIG192.168.2.15 9800 midi   note_on   midi_channel 1    low   0x3b    hi    0x3b    low_velocity   0x01   hi_velocity    0x70    end192.168.2.15 9900 :FCL08CONFIG