#!/bin/bashactor -d1 -n opto_init -h 10.5.1.9 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.9 9900 :FCA10.5.1.9 9900 :FEA10.5.1.9 9900 :FCGFFFF10.5.1.9 9900 :FEGFFCONFIG