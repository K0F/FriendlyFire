#!/bin/bashactor -d1 -n violin_m3_g4 -h 10.5.1.14 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 2    low   0x43    hi    0x43    low_velocity   0x01   hi_velocity    0x70    end10.5.1.14 9900 :FCK04CONFIG