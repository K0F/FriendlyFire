#!/bin/bash
mount maiden:/actors /mnt
cp -Rf /mnt/* /actors/maiden/
umount /mnt
