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
        s=`grep waiting tst_${nm}*.log`
    done
}

wait_for_start_zdo() {
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
        s=`grep Device tst_${nm}*.log`
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
    kill $endPID $coordPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT


rm -f *.log *.pcap *.dump core

echo "run ns"

../../../devtools/network_simulator/network_simulator --nNode=2 --pipeName=/tmp/st >ns.txt 2>&1 &
PipePID=$!
sleep 1

echo "run coordinator"
./zdo_start_zc_s05 /tmp/st0.write /tmp/st0.read &
coordPID=$!
wait_for_start_zdo zc

echo ZC STARTED OK

echo "run ze"
./start_ze_s05 /tmp/st1.write /tmp/st1.read &
endPID=$!
wait_for_start ze

echo ZE STARTED OK

sleep 20

if [ -f core ]
then
    echo 'Somebody has crashed'
fi

echo 'shutdown...'
killch


set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 c.pcap
../../../devtools/dump_converter/dump_converter -ns $2 ze.pcap

echo 'Now verify traffic dump, please!'

if grep -A 1 "pan desc:" tst_ze*.log
then
  echo "DONE. Check Pan Descriptor"
else
  echo "ERROR. TEST FAILED!!!"
fi

