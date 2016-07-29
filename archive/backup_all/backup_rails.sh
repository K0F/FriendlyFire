#!/bin/bash
mount rails:/actors /mnt
cp -Rf /mnt/* /actors/rails/
umount /mnt
