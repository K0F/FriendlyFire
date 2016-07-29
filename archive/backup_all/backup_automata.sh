#!/bin/bash
mount automata:/actors /mnt
cp -Rf /mnt/* /actors/automata/
umount /mnt
