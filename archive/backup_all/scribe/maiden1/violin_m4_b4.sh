#!/bin/bashactor -d1 -n violin_m4_b4 -h 10.5.1.14 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 3    low   0x47    hi    0x47    low_velocity   0x01   hi_velocity    0x70    end10.5.1.14 9900 :FCL0ACONFIG