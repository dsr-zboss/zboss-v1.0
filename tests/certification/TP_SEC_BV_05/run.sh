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
    kill $ze1PID $ze2PID $ze3PID $router2PID $router1PID $coordPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT


rm -f *.log *.pcap *.dump core

echo "run ns"

rm -f /tmp/zv*
../../../devtools/network_simulator/network_simulator --nNode=6 --pipeName=/tmp/zv --xgml=TPSECBV05.xgml >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo "run coordinator"
./tp_sec_bv_05_zc /tmp/zv0.write /tmp/zv0.read &
coordPID=$!
wait_for_start zc

echo ZC STARTED OK

echo "run zr1"
./tp_sec_bv_05_zr1 /tmp/zv1.write /tmp/zv1.read &
router1PID=$!
wait_for_start zr1

echo ZR1 STARTED OK

echo "run zr2"
./tp_sec_bv_05_zr2 /tmp/zv2.write /tmp/zv2.read &
router2PID=$!
wait_for_start zr2

echo ZR2 STARTED OK

echo "run zed1"
./tp_sec_bv_05_zed1 /tmp/zv3.write /tmp/zv3.read &
ze1PID=$!
wait_for_start zed1

echo ZED1 STARTED OK

echo "run zed2"
./tp_sec_bv_05_zed2 /tmp/zv4.write /tmp/zv4.read &
ze2PID=$!
wait_for_start zed2

echo ZED2 STARTED OK

sleep 10

echo "run zed3"
./tp_sec_bv_05_zed3 /tmp/zv5.write /tmp/zv5.read &
ze2PID=$!
wait_for_start zed3

echo ZED3 STARTED OK


sleep 80

         
if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 c.pcap
../../../devtools/dump_converter/dump_converter -ns $2 r1.pcap
../../../devtools/dump_converter/dump_converter -ns $3 r2.pcap
../../../devtools/dump_converter/dump_converter -ns $4 zed1.pcap
../../../devtools/dump_converter/dump_converter -ns $5 zed2.pcap
../../../devtools/dump_converter/dump_converter -ns $6 zed3.pcap


echo 'Now verify traffic dump, please!'

