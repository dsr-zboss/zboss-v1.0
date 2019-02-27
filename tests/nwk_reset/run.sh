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
            echo 'Somebody has crashed?'
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
    kill $c_pid $ed_pid $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.dump

echo "run network simulator"
../../devtools/network_simulator/network_simulator --nNode=2 --pipeName=/tmp/znode &
ns_pid=$!
sleep 1

echo "run coordinator"
./nwk_reset_zc /tmp/znode0.write /tmp/znode0.read &
c_pid=$!
wait_for_start zc
echo ZC STARTED OK

echo "run dut zed"
./nwk_reset_ze /tmp/znode1.write /tmp/znode1.read &
ed_pid=$!
wait_for_start zed
echo ED STARTED OK

sleep 30

killch

set - `ls *dump`
../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../devtools/dump_converter/dump_converter -ns $2 ze.pcap

if grep "RESET CALLBACK status 0" zdo_zed*.log
then
  echo "Test passed"
else
  echo "Test failed. Analyze log files"
fi
