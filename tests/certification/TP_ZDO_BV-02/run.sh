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

PIPE_NAME=/tmp/aaa

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
    kill $endPID $rPID $coordPID $PipePID
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
../../../devtools/network_simulator/network_simulator --nNode=3 --pipeName=${PIPE_NAME} 1>sim_log.txt 2>&1 &
PipePID=$!

sleep 5

echo "run coordinator"
./tp_zdo_bv_02_zc ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start zc
echo ZC STARTED OK

echo "run zed"
./tp_zdo_bv_02_zed ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
endPID=$!
wait_for_start zed
echo ZED STARTED OK

echo "run router"
./tp_zdo_bv_02_zr ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
rPID=$!
wait_for_start zr
echo ZR STARTED OK

sleep 60

echo shutdown...
killch

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 zed.pcap
../../../devtools/dump_converter/dump_converter -ns $3 zr.pcap

echo 'Now verify traffic dump, please!'
