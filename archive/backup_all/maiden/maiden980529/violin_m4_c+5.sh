#!/bin/bash# hand left & right(4)(7) offactor -d1 -n violin_m4_c+5 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 3    low   0x49    hi    0x49    low_velocity   0x01   hi_velocity    0x70    end10.5.1.14 9900 :FCL90CONFIG