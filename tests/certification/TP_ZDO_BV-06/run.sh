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
        s=`grep Device tp_zdo_bv_06_${nm}*.log`
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
./tp_zdo_bv_06_DUTZC ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start DUTZC

echo ZC STARTED OK
sleep 1

echo "run zed1"
./tp_zdo_bv_06_gZED1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
end1PID=$!
wait_for_start gZED1

echo gZED1 STARTED OK
sleep 1

echo "run zed2"
./tp_zdo_bv_06_gZED2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
end2PID=$!
wait_for_start gZED2

echo gZED2 STARTED OK


sleep 180

echo shutdown...
killch

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 ze1.pcap
../../../devtools/dump_converter/dump_converter -ns $3 ze2.pcap


if grep "Test is finished, status OK" tp_zdo_bv_06_gZED1*.log
then
  echo "DONE. TEST PASSED!!!"
else
  echo "ERROR. TEST FAILED!!!"
fi

