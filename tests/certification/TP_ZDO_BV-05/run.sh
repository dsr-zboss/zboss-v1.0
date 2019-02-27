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

PIPE_NAME=/tmp/st

wait_for_start() {
    nm=$1
    s=''
    while [ A"$s" = A ]
    do
        sleep 1
        if [ -f core ]
        then
            echo 'Somebody has crashed (ns?)'
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
    kill $ed1PID $ed2PID $coordPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT



rm -f *.log *.pcap *.dump core

echo "run ns"
../../../devtools/network_simulator/network_simulator --nNode=3 --pipeName=${PIPE_NAME}  1>sim_log.txt 2>&1 &
PipePID=$!
echo sim started, pid $PipePID

sleep 5

echo "run coordinator"
./TP_ZDO_BV_05_DUTZC ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!
wait_for_start zc

echo ZC STARTED OK
sleep 1



echo "run ze1"
./TP_ZDO_BV_05_ZED1 ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
ed1PID=$!
wait_for_start zed1

echo ZE1 STARTED OK
sleep 1

echo "run ze2"
./TP_ZDO_BV_05_ZED2 ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
ed2PID=$!
wait_for_start zed2

echo ZE2 STARTED OK
sleep 1


sleep 20

echo shutdown...
killch

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 ze1.pcap

echo 'Now verify traffic dump, please!'

if grep "Test status: OK" zdo_zed1*.log
then
  echo "DONE. TEST PASSED!!!"
else
  echo "ERROR. TEST FAILED!!!"
fi



