#!/bin/bash# laser plays on any noteactor -d0 -n stealth_laser_random -h 10.5.1.12 -p 9850 << CONFIG10.5.1.12 9800 midi   note_on   midi_channel 0    low   0x01    hi    0x7f    low_velocity   0x01   hi_velocity    0x7f    endif %frame -gt 54000	{	shell makerandom 500 1000 > currentrandom.txt	shell hpcp -d0 -l -h 10.5.1.12 -p 9850 -f currentrandom.txt	set frame %random	}set frame %frame+%random10.5.1.17 9901 : %frame se pl\rCONFIG