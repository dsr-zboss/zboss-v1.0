#!/bin/sh
#
#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
#
# Purpose:
#

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed'
            killch
            exit 1
        fi
        s=`grep Device zdo_${nm}*.log`
    done
    if echo $s | grep OK
    then
        return
    else
        echo $s
        killch
        exit 1
    fi
}

killch() {
    kill $pid1 $pid2 $pid3 $pid4 $pid5 $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.pcap *.dump core

echo "run ns"
../../devtools/network_simulator/network_simulator --nNode=5 --pipeName=/tmp/zt --xgml=tpprobv23.xgml >ns.txt 2>&1 &
ns_pid=$!
sleep 1

echo "run coordinator"
./nwk_report_update_zc1 /tmp/zt0.write /tmp/zt0.read &
pid1=$!
wait_for_start zc1
echo ZC1 STARTED OK

echo "run zr1"
./nwk_report_update_zr1 /tmp/zt1.write /tmp/zt1.read &
pid2=$!
wait_for_start zr1
echo ZR1 STARTED OK

echo "run zr2"
./nwk_report_update_zr2 /tmp/zt2.write /tmp/zt2.read &
pid3=$!
wait_for_start zr2
echo ZR2 STARTED OK

echo "run zc2"
./nwk_report_update_zc2 /tmp/zt3.write /tmp/zt3.read &
pid4=$!
wait_for_start zc2
echo ZC2 STARTED OK

sleep 2

echo "run zr3 - it must not start"
./nwk_report_update_zr3 /tmp/zt4.write /tmp/zt4.read &
pid5=$!

sleep 20

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../devtools/dump_converter/dump_converter -ns $1 c1.pcap
../../devtools/dump_converter/dump_converter -ns $2 r1.pcap
../../devtools/dump_converter/dump_converter -ns $3 r2.pcap
../../devtools/dump_converter/dump_converter -ns $4 c2.pcap
../../devtools/dump_converter/dump_converter -ns $5 r3.pcap

echo 'Now verify traffic dump, please!'

