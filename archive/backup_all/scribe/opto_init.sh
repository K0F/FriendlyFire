#!/bin/bashactor -d1 -n opto_init -h 10.5.1.14 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.14 9900 :FAA#10.5.1.14 9900 :FCA10.5.1.14 9900 :FDA10.5.1.14 9900 :FEA10.5.1.14 9900 :FAG000F#10.5.1.14 9900 :FCGFFFF10.5.1.14 9900 :FDG000010.5.1.14 9900 :FEGFFCONFIG