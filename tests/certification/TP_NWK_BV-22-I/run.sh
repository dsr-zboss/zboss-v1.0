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
    kill $pid1 $pid2 $pid3 $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.pcap *.dump core

echo "run ns"

../../../devtools/network_simulator/network_simulator --nNode=3 --pipeName=/tmp/zt --xgml=tp_nwk_bv_22_i.xgml >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo "run coordinator"
./tp_nwk_bv_22_i_zc /tmp/zt0.write /tmp/zt0.read &
pid1=$!
wait_for_start zc
echo ZC STARTED OK

echo "run zr"
./tp_nwk_bv_22_i_zr /tmp/zt1.write /tmp/zt1.read &
pid2=$!
wait_for_start zr
echo ZR STARTED OK

echo "run zed"
./tp_nwk_bv_22_i_zed /tmp/zt2.write /tmp/zt2.read &
pid3=$!
wait_for_start zed
echo ZED STARTED OK

sleep 30

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 zr.pcap
../../../devtools/dump_converter/dump_converter -ns $3 zed.pcap

echo 'Now verify traffic dump, please!'

