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
        s=`grep Device zdo_start_${nm}*.log`
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
    kill $router1PID $router2PID $coordPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

export LD_LIBRARY_PATH=`pwd`/../../../ns/ns-3.7/build/debug
rm -f *.log *.pcap *.dump

echo "run ns-3"
../../devtools/network_simulator/network_simulator --nNode=3 --pipeName=/tmp/aaa &
PipePID=$!

sleep 5

echo "run coordinator"
./zdo_start_zc /tmp/aaa0.write /tmp/aaa0.read &
coordPID=$!
wait_for_start zc

echo gZC STARTED OK
sleep 1

echo "run router1"
./zdo_start_zr1 /tmp/aaa1.write /tmp/aaa1.read &
router1PID=$!
wait_for_start zr1

echo ZR STARTED OK
sleep 1

echo "run router2"
./zdo_start_zr2 /tmp/aaa2.write /tmp/aaa2.read &
router2PID=$!
wait_for_start zr2

echo ZR STARTED OK
sleep 1

sleep 150

echo shutdown...
killch

set - `ls *dump`
../../devtools/dump_converter/dump_converter -ns $1 c.pcap
../../devtools/dump_converter/dump_converter -ns $2 r.pcap
