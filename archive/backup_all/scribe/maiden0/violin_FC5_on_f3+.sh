#!/bin/bashactor -d1 -n violin_FC5_on_f3+ -h 192.168.2.14 -p 9850 << CONFIG192.168.2.15 9800 midi   note_on   midi_channel 0    low   0x36    hi    0x36    low_velocity   0x01   hi_velocity    0x70    end192.168.2.15 9900 :FCK20CONFIG