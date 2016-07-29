#!/bin/bash
mount fire:/actors /mnt
cp -Rf /mnt/* /actors/fire/
umount /mnt
