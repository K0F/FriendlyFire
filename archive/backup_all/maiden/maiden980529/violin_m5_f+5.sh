#!/bin/bash# fan 1(13) onactor -d1 -n violin_m5_f+5 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 4    low   0x4e    hi    0x4e    low_velocity   0x01   hi_velocity    0x70    end10.5.1.14 9900 :FCL2000CONFIG