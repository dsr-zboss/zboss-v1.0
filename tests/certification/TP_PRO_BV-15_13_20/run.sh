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


rm -f *.log *.pcap *.dump

PIPE_NAME=/tmp/zt`whoami`
echo "run ns"
../../../devtools/network_simulator/network_simulator --nNode=4 --Join=1 --PipeName=${PIPE_NAME} >upipe.txt 2>&1 &
PipePID=$!

sleep 5

echo "run coordinator"
./tp_pro_bv_15_zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start zc

echo ZC STARTED OK
sleep 1

echo "run zr1"
./tp_pro_bv_15_zr1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
router1PID=$!
wait_for_start zr1

echo ZR1 STARTED OK
sleep 1

echo "run zr2"
./tp_pro_bv_15_zr2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
router2PID=$!
wait_for_start zr2

echo ZR2 STARTED OK
sleep 1

echo "run ze1"
./tp_pro_bv_15_zed1 ${PIPE_NAME}3.write ${PIPE_NAME}3.read &
ze1PID=$!
wait_for_start zed1

echo ZE STARTED OK - done!

sleep 30

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 c.pcap
../../../devtools/dump_converter/dump_converter -ns $2 r1.pcap
../../../devtools/dump_converter/dump_converter -ns $3 r2.pcap
../../../devtools/dump_converter/dump_converter -ns $4 ed.pcap

echo 'Now verify traffic dump, please!'


echo shutdown...
killch

