#!/bin/bash# all off high pressureactor -d1 -n violin_m5_e6 -h 10.5.1.9 -p 9850 << CONFIG10.5.1.9 9800 midi   note_on   midi_channel 4    low   0x58    hi    0x58    low_velocity   0x01   hi_velocity    0x70    end10.5.1.9 9900 :FCLFFFF10.5.1.9 9900 :FCK8000CONFIG