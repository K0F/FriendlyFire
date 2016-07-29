#!/bin/bash
# create link in /usr/sbin for unicom software
if [ $# -lt 2 ]
then
  echo 'Calling format is: create_link.sh <linkname> <path to executeable>'
else
  echo 'Creating link /usr/sbin/'$1' pointing to '$2
  echo 'Ctrl-C to quit or enter to continue'
  read
  rm -i /usr/sbin/$1
  ln -s $2 /usr/sbin/$1
  ls -l /usr/sbin/$1
fi

