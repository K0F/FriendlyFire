#!/bin/bash
mount scribe:/actors /mnt
cp -Rf /mnt/* /actors/scribe/
umount /mnt
