#!/bin/sh

while :
do
    ### Start the ntpd daemon ###
    /etc/init.d/ntp start >/dev/null
    ### Set system time to hardware clock ###
    hwclock --systohc --localtime >/dev/null
    hwclock --hctosys >/dev/null

    ### Check if the client sync with the NTP server (MAX 60sec)###
    timeout=1
    for i in `seq 30`
    do
        ntpq -c 'rv 0' | grep 'sync_ntp' >/dev/null
        if [ "$?" = "0" ]; then
            timeout=0
            break
        fi
        sleep 2
    done

    ### When sync with the NTP server,                                                  ###
    ### set the system clock to the hardware clock after the sink operation stabilizes. ###
    if [ "$timeout" = "0" ]; then
        ntp-wait
        hwclock --systohc --localtime >/dev/null
        hwclock --hctosys >/dev/null
    fi

    ### Kill the ntpd daemon ###
    killall ntpd

    # 24h sleep
    sleep 86400 

done



