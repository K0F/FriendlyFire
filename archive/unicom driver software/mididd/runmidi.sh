#!/bin/bash
#shell script to start the midi dd
echo "Starting mididd with args -d0", $1, $2
cd /midi
echo " " >>mididd.diags
echo "Starting mididd with args -d0", $1, $2 >>mididd.diags
date >>mididd.diags
exec mididd -d0 $1 $2 
echo " "

