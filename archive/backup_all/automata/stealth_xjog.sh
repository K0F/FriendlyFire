#!/bin/shactor -d0 -n stealth_xjog -h 10.5.1.12 -p 9850 << CONFIG10.5.1.12 9800 midi   note_on   midi_channel 1   low   0x01   hi    0x04   low_velocity   0x00   hi_velocity    0x7f   endif %mnote -lt 3	10.5.1.17 9900 :ax jg200	if %mnote -gt 2	10.5.1.17 9900 :ax jg-200	set xlimit %mnoteCONFIG