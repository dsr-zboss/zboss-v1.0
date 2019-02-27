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
    kill $c_pid $ed1_pid $ed2_pid $ns_pid
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.dump

echo "run network simulator"
../../../devtools/network_simulator/network_simulator --nNode=3 --pipeName=/tmp/aaa &
ns_pid=$!
sleep 1

echo "run coordinator"
./tp_pro_bv_27_gZC /tmp/aaa0.write /tmp/aaa0.read &
c_pid=$!
wait_for_start zc
echo ZC STARTED OK

echo "run dut zed1"
./tp_pro_bv_27_DUTZED1 /tmp/aaa1.write /tmp/aaa1.read &
ed1_pid=$!
wait_for_start zed1
echo ED1 STARTED OK

echo "run dut zed2"
./tp_pro_bv_27_DUTZED2 /tmp/aaa2.write /tmp/aaa2.read &
ed2_pid=$!
wait_for_start zed2
echo ED2 STARTED OK

sleep 60

killch

if grep "Device STARTED OK" zdo_zed1*.log
then
  if grep "Device STARTED OK" zdo_zed2*.log
  then
    echo "DONE. TEST PASSED!!!"
  else
    echo "ERROR. TEST FAILED!!!"
  fi
else
  echo "ERROR. TEST FAILED!!!"
fi

set - `ls *dump`
../../../devtools/dump_converter/dump_converter -ns $1 zc.pcap
../../../devtools/dump_converter/dump_converter -ns $2 zed1.pcap
../../../devtools/dump_converter/dump_converter -ns $3 zed2.pcap

echo 'Now verify traffic dump, please!'
