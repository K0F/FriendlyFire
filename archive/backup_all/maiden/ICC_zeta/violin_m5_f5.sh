#!/bin/bash# All on except 15 = low pressureactor -d1 -n violin_m5_f5 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 4    low   0x4d    hi    0x4d violin_m5_   low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FCL800010.5.1.9 9900 :FCK7FFFCONFIG