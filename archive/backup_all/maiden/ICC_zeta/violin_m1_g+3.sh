#!/bin/bash# pelvic roll(2) offactor -d1 -n violin_m1_g+3 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 0    low   0x38    hi    0x38    low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FCL04CONFIG