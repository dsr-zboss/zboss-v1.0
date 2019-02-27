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
    kill $pid1 $pid2 $pid3 $pid4 $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT



rm -f *.log *.pcap *.dump core

echo "run ns"

../../../devtools/network_simulator/network_simulator --nNode=4 --pipeName=/tmp/zt --xgml=tpprobv23.xgml >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo "run coordinator"
./tp_pro_bv_23_zc1 /tmp/zt0.write /tmp/zt0.read &
pid1=$!
wait_for_start zc1

echo ZC1 STARTED OK

echo "run zr2"
./tp_pro_bv_23_zr2 /tmp/zt1.write /tmp/zt1.read &
pid2=$!
wait_for_start zr2

echo ZR2 STARTED OK

sleep 2

echo "run zc2"
./tp_pro_bv_23_zc2 /tmp/zt2.write /tmp/zt2.read &
pid3=$!
wait_for_start zc2
echo ZC2 STARTED OK

sleep 2

echo "run zr3 - it must not start"
./tp_pro_bv_23_zr3 /tmp/zt3.write /tmp/zt3.read &
pid4=$!

sleep 90

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 c1.pcap
../../../devtools/dump_converter/dump_converter -ns $2 r2.pcap
../../../devtools/dump_converter/dump_converter -ns $3 c2.pcap
../../../devtools/dump_converter/dump_converter -ns $4 r3.pcap

echo 'Now verify traffic dump, please!'

