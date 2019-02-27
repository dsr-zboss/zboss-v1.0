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
    kill $ze1PID $router2PID $router1PID $coordPID $PipePID
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
../../../devtools/network_simulator/network_simulator --nNode=5 --Join=1 --pipeName=/tmp/zt >upipe.txt 2>&1 &
PipePID=$!

sleep 5

echo "run coordinator"
./tp_pro_bv_15_21_zc /tmp/zt0.write /tmp/zt0.read &
coordPID=$!
wait_for_start zc

echo ZC STARTED OK
sleep 1

echo "run zr1"
./tp_pro_bv_15_21_zr1 /tmp/zt1.write /tmp/zt1.read &
router1PID=$!
wait_for_start zr1

echo ZR1 STARTED OK
sleep 1

echo "run zr2"
./tp_pro_bv_15_21_zr2 /tmp/zt2.write /tmp/zt2.read &
router2PID=$!
wait_for_start zr2

echo ZR2 STARTED OK
sleep 1

echo "run ze1"
./tp_pro_bv_15_21_zed1 /tmp/zt3.write /tmp/zt3.read &
ze1PID=$!
wait_for_start zed1

echo ZE STARTED OK - done!

sleep 10

echo shutdown...
killch

