#!/bin/bash
# first test of an immediate actor
./actor1 -d1 -n table3_script -h 192.168.2.3 -p 9850 << CONFIG
localhost 0 immediate
   init_delay 0
   loop_delay 0
   iterations 0
   end



	#p0-on_p2-on_p5-on
	shell /bin/echo -n "\332" >/dev/lp1
	set video_mode 1
	192.168.2.14 9900 :5se1801pl\r

shell sleep 3

	#p2_on_p5_on
	shell /bin/echo -n "\333" >/dev/lp1

shell sleep 27

	192.168.2.14 9900 :64 rb\r

shell sleep 29

	192.168.2.14 9900 :1802se3200pl\r

shell sleep 46

	192.168.2.14 9900 :3261se5551pl\r

shell sleep 76

	192.168.2.14 9900 :5552se7973pl\r

shell sleep 80

	192.168.2.14 9900 :7974se10303pl\r

shell sleep 67

	#p0-on_p2_on_p5_on
	shell /bin/echo -n "\332" >/dev/lp1

shell sleep 10

	#p0-on_p1-on_p4-on
	shell /bin/echo -n "\354" >/dev/lp1
	set video_mode 0
	192.168.2.14 9900 :1se\r

shell sleep 5
	
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 5

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 2

	192.168.2.14 9900 :29082sepl\r

shell sleep 8

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p0-on_p1-on_p4-on
	shell /bin/echo -n "\354" >/dev/lp1

shell sleep 10

	#p0-on_p2-on_p5-on
	shell /bin/echo -n "\332" >/dev/lp1

shell sleep 2

	192.168.2.14 9900 :16142se19010pl\r
	set video_mode 1

shell sleep 5
	
	#p2_on_p5_on
	shell /bin/echo -n "\333" >/dev/lp1

shell sleep 89

	192.168.2.14 9900 :19011se23851pl\r

shell sleep 61

	192.168.2.14 9900 :23852se25287pl\r

shell sleep 49

	192.168.2.14 9900 :25288se25775pl\r

shell sleep 17

	192.168.2.14 9900 :25776se26775pl\r

shell sleep 34

	192.168.2.14 9900 :26776se27638pl\r

shell sleep 19

	192.168.2.14 9900 :27639se29043pl\r

shell sleep 38

	#p0-on_p2-on_p5-on
	shell /bin/echo -n "\332" >/dev/lp1

shell sleep 10

	#p0-on_p1-on_p4-on
	shell /bin/echo -n "\354" >/dev/lp1
	set video_mode 0

shell sleep 5

	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1
	

shell sleep 5

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 2

	192.168.2.14 9900 :34000sepl\r
shell sleep 8

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p1-on_p4-on
	shell /bin/echo -n "\355" >/dev/lp1

shell sleep 10

	#p1-on_p3-on_p4-on
	shell /bin/echo -n "\345" >/dev/lp1
	#p0-on_p1-on_p4-on
	shell /bin/echo -n "\354" >/dev/lp1

shell sleep 10

	#p0-on_p2-on_p5-on
	shell /bin/echo -n "\332" >/dev/lp1

CONFIG
