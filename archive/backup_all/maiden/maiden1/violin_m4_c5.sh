#!/bin/bashactor -d1 -n violin_m4_c5 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.15 9800 midi   note_on   midi_channel 3    low   0x48    hi    0x48    low_velocity   0x01   hi_velocity    0x70    end10.5.1.15 9900 :FCK90CONFIG