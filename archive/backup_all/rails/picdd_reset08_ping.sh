#!/bin/shactor -d0 -n picdd_reset08_ping -h 10.5.1.10 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 0   end10.5.1.10 9700 :ping 08shell sleep 1CONFIG