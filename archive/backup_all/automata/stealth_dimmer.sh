#!/bin/shactor -d0 -n stealth_dimmer -h 10.5.1.12 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 0   end10.5.1.10 9901 :F60\rD13-15@25\rG\rshell sleep 2510.5.1.10 9901 :F60\rD13-15@50\rG\rshell sleep 60CONFIG