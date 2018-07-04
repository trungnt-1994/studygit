#!/bin/sh
# This is a toshiba tvalz driver installation sample.
# Please run this file with root privilege.

module="tvalz"
device="tvalz"
##mode="775"
# 1. Install process
# Get a right group
##if grep '^staff' /etc/group > /dev/null; then
##	group="staff"
##else
##	group="root"
##fi

#Get major number.
major=`cat /proc/devices | awk '{if($2=="tvalz") {print $1}}'`
#echo $major

#Create device node which can be accessed by application.
mknod /dev/${device}0 c $major 1


#give appropriate group/perssions
##chgrp $group /dev/${device}0
##chmod $mode  /dev/${device}0 

# 2. Uninstall process
#Remove the created device node.
#rm -f /dev/${device}0
