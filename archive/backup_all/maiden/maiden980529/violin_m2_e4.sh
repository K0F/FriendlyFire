#!/bin/bash# hand l, arm l(4)(5) onactor -d1 -n violin_m2_e4 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 1    low   0x40    hi    0x40    low_velocity   0x01   hi_velocity    0x70    end10.5.1.14 9900 :FCK30CONFIG