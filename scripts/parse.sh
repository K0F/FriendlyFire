#!/bin/bash
while read -r LINE; do
   if [[ $LINE == *"Note on"* ]]
	then
		if [[ $LINE == *"note 35"* ]]
		then
		echo 1
		./dbuscontrol.sh seek -100000000
		fi

		if [[ $LINE == *"note 38"* ]]
		then
		echo 2
		./dbuscontrol.sh setposition 1
		fi

		if [[ $LINE == *"note 42"* ]]
		then
		echo 3
		./dbuscontrol.sh seek 100000000
		fi

		if [[ $LINE == *"note 49"* ]]
		then
		echo 4
		./dbuscontrol.sh seek 1000000000
		fi

		if [[ $LINE == *"note 46"* ]]
		then
		echo 5
		./dbuscontrol.sh seek 1000000000
		fi

		echo $LINE
fi
done
