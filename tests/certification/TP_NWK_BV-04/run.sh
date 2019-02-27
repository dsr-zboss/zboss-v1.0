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
        s=`grep Device tp_nwk_bv_04_${nm}*.log`
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
    kill $routerPID $coordPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

export LD_LIBRARY_PATH=`pwd`/../../../../ns/ns-3.7/build/debug
rm -f *.log *.pcap *.dump

echo "run ns-3"
../../../devtools/network_simulator/network_simulator --nNode=2 --pipeName=/tmp/zt &
PipePID=$!

sleep 5

echo "run coordinator"
./tp_nwk_bv_04_gZC /tmp/zt0.write /tmp/zt0.read &
coordPID=$!
wait_for_start gZC

echo gZC STARTED OK
sleep 1

echo "run router"
./tp_nwk_bv_04_ZR /tmp/zt1.write /tmp/zt1.read &
routerPID=$!
wait_for_start ZR

echo ZR STARTED OK
sleep 1

sleep 150

echo shutdown...
killch

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 c.pcap
../../../devtools/dump_converter/dump_converter -ns $2 r.pcap

