#!/bin/shactor -d0 -n automata_script -h 10.5.1.12 -p 9850 << CONFIGlocalhost 0 immediate   init_delay 0   loop_delay 0   iterations 1   end10.5.1.12 9900 :mn 90 7f 01# midi note to send when move is finished10.5.1.12 9900 :aa ac200,200,200;10.5.1.12 9900 :aa vl500,500,400;10.5.1.12 9900 :aa mt0,0,0; gd id# go home all axes10.5.1.12 9900 :aa mt4000,0,1610; gd idshell /sound/rsynth/say west# go west      CONFIG