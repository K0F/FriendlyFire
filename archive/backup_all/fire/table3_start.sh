#!/bin/shactor -d0 -n table3_start.sh -h 10.5.1.15 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   endshell echo starting table3 actors >>/var/log/table3.logshell date >>/var/log/table3.logshell /actors/video_pl.shshell sleep 15#shell /actors/pad1.sh &#shell /actors/pad2.sh &#shell /actors/pad3.sh &#shell /actors/pad4.sh &#shell /actors/pad5.sh &shell /actors/table3_timeline.sh &#shell msleep 500#shell /actors/check_and_restart.sh &CONFIG