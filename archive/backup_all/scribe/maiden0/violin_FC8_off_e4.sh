#!/bin/bashactor -d1 -n violin_FC8_off_e4 -h 192.168.2.14 -p 9850 << CONFIG192.168.2.15 9800 midi   note_on   midi_channel 1    low   0x40    hi    0x40    low_velocity   0x01   hi_velocity    0x70    end192.168.2.15 9900 :FCL100CONFIG