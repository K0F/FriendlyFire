#!/bin/shactor -d0 -n automata_midi3 -h 10.5.1.12 -p 9850 << CONFIG10.5.1.12 9901 midi   note_on   midi_channel 0    low   0x7f   hi    0x7f   low_velocity   0x01   hi_velocity    0x7f    end#10.5.1.12 9901 :aa vl350,350,350;#10.5.1.12 9902 : 60 spif %mvel -eq 6	{	10.5.1.12 9901 :aa ml12000,0,0; gd id	10.5.1.12 9902 : 15070 se  13986 mr\r		10.5.1.12 9901 :mn 90 7f 07	# light 6	}if %mvel -eq 7	{	10.5.1.12 9901 :aa ml-4000,0,0; gd id	10.5.1.12 9902 : 13986 se 14346 mf\r		10.5.1.12 9901 :mn 90 7f 08	}if %mvel -eq 8	{	10.5.1.12 9901 :aa ml0,4000,0; gd id	10.5.1.12 9902 : 15795 se 16155 mf\r		10.5.1.12 9901 :mn 90 7f 09	}if %mvel -eq 9	{	10.5.1.12 9901 :aa ml0,-4000,0; gd id	10.5.1.12 9902 : 16155 se 15795 mr\r		10.5.1.12 9901 :mn 90 7f 0a	}if %mvel -eq 10	{	10.5.1.12 9901 :aa ml0,-8000,0; gd id	10.5.1.12 9902 : 17235 se 16515 mr\r		10.5.1.12 9901 :mn 90 7f 0b	}if %mvel -eq 11	{	10.5.1.12 9901 :aa ml-8000,0,0; gd id	10.5.1.12 9902 : 14346 se 15070 mf\r		10.5.1.12 9901 :mn 90 7f 06	}#if %mvel -eq 12#	{#	10.5.1.12 9901 :ax km ay km az km#	10.5.1.12 9901 :ax hm0 ay hm0 az hm0; gd id#	10.5.1.12 9902 : \r	#	10.5.1.12 9901 :mn 90 7f 06#	}CONFIG