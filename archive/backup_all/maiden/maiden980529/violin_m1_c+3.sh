#!/bin/bash# foot stretch onactor -d1 -n violin_m1_c+3 n -h 10.5.1.9 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 0    low   0x31    hi    0x31    low_velocity   0x01   hi_velocity    0x70    end10.5.1.14 9900 :FCK400CONFIG