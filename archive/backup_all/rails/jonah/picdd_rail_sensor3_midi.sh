#!/bin/shactor -d1 -n picdd_rail_sensor3_midi -h 10.5.1.3 -p 9850 << CONFIG10.5.1.14 9800 midi   note_on   midi_channel 0    low   0x03   hi    0x03   low_velocity   0x00    hi_velocity    0x7f    endset sensor3_midi %mvelif %sensor3_midi -lt 80   set go_middle_y 0CONFIG