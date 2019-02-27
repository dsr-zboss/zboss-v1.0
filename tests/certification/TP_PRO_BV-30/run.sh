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

PIPE_NAME=/tmp/bbb

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        s=`grep Device tp_pro_bv_30_${nm}*.log`
    done
    if echo $s | grep OK
    then
        return
    else
     if echo ${nm} | grep ZR2
     then
      echo zr2 - expected fail
     else
        echo $s
        killch
        exit 1
     fi
    fi
}

killch() {
    kill $end1PID $end2PID $coordPID $PipePID
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
./tp_pro_bv_30_gZC ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start gZC

echo gZC STARTED OK
sleep 1

echo "run ZR1"
./tp_pro_bv_30_DUTZR1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
end1PID=$!
wait_for_start DUTZR1

echo DURZR1 STARTED OK
sleep 1

echo "run ZR2"
./tp_pro_bv_30_DUTZR2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
end2PID=$!
wait_for_start DUTZR2

echo DUTZR2 STARTED OK


sleep 45

echo shutdown...
killch

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 ze1.pcap
../../../devtools/dump_converter/dump_converter -ns $3 ze2.pcap


echo "Done. Please, verify traffic dump."
