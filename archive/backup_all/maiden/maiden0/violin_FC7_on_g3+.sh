#!/bin/bashactor -d1 -n violin_FC7_on_g3+ -h 10.5.1.14 -p 9850 << CONFIG10.5.1.15 9800 midi   note_on   midi_channel 0    low   0x38    hi    0x38    low_velocity   0x01   hi_velocity    0x70    end10.5.1.15 9900 :FCK80CONFIG