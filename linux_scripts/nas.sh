#!/bin/bash
if [ "$1" == "mount" ] ; then
	echo "MOUNT"
	mount -t cifs //[IP addr]/share /home/sjlee/nas/ -o user=sjlee,rw,vers=1.0
else
	echo "UNMOUNT"
	umount /home/sjlee/nas
fi
