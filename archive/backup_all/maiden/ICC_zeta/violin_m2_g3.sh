#!/bin/bash# all off low pressureactor -d1 -n violin_m2_g3 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 1    low   0x37    hi    0x37    low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FCLFFFF10.5.1.9 9900 :FCK4000CONFIG