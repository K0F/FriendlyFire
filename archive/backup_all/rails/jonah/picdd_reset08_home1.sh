#!/bin/shactor -d0 -n picdd_reset08_home1 -h 10.5.1.3 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   endshell picdd_reset08_stat.sh &shell msleep 15010.5.1.3 9700 :stat 08shell msleep 100if %location08 -le 16   {	10.5.1.3 9700 :vel 08 200000	10.5.1.3 9700 :vmode 08 rev	}shell picdd_reset08_center.sh &shell picdd_reset08_ping.sh &shell msleep 500#10.5.1.3 9700 :home1 0810.5.1.3 9700 :vel 08 20000010.5.1.3 9700 :vmode 08 forCONFIG