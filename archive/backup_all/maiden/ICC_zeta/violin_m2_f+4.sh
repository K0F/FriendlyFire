#!/bin/bash# hand l, arm l, hand r, arm r (4)(5)(6)(7) onactor -d1 -n violin_m2_f+4 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 1    low   0x42    hi    0x42    low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FCKF0CONFIG