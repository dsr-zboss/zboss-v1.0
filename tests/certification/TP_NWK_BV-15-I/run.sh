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

../../../devtools/network_simulator/network_simulator --nNode=4 --pipeName=/tmp/zt --xgml=tp_nwk_bv_15_i.xgml >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo "run coordinator"
./tp_nwk_bv_15_i_zc /tmp/zt0.write /tmp/zt0.read &
pid1=$!
wait_for_start zc

echo ZC STARTED OK

echo "run zr"
./tp_nwk_bv_15_i_zr /tmp/zt1.write /tmp/zt1.read &
pid2=$!
wait_for_start zr

echo ZR STARTED OK

echo "run zed1"
./tp_nwk_bv_15_i_zed1 /tmp/zt2.write /tmp/zt2.read &
pid3=$!
wait_for_start zed1
echo ZED1 STARTED OK

echo "run zed2"
./tp_nwk_bv_15_i_zed2 /tmp/zt3.write /tmp/zt3.read &
pid4=$!
wait_for_start zed2
echo ZED2 STARTED OK

sleep 90

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 zr.pcap
../../../devtools/dump_converter/dump_converter -ns $3 zed1.pcap
../../../devtools/dump_converter/dump_converter -ns $4 zed2.pcap

echo 'Now verify traffic dump, please!'

