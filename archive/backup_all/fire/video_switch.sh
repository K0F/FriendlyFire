#!/bin/bashactor -d1 -n video_switch.sh -h 10.5.1.3 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.3 9900 :@\rshell sleep 210.5.1.3 9900 :@\rshell sleep 2 CONFIG