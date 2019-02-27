#!/bin/sh
#
#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
#
# Purpose: NWK discovery test using ns-3 sim and native Linux executable
#

export LD_LIBRARY_PATH=`pwd`/../../../ns/ns-3.7/build/debug

rm -f *.log *.pcap *.dump

echo "run ns"
../../devtools/network_simulator/network_simulator --nNode=2 --pipeName=/tmp/st &
PipePID=$!

sleep 5

echo "run coordinator"
./nwk_formation_test /tmp/st0.write /tmp/st0.read &
coordPID=$!

sleep 30

echo "run route discovery"
#./nwk_disc_test /tmp/zudp1.write /tmp/zudp1.read &
#./nwk_disc_test /tmp/st1.write /tmp/st1.read -rejoin &
./nwk_disc_test /tmp/st1.write /tmp/st1.read -orphan_rejoin &
discPID=$!

sleep 60

echo "kill pids"
kill $discPID
kill $coordPID
kill $PipePID

echo "waiting for disc node..."
wait $discPID
echo "waiting for coordinator node..."
wait $coordPID
echo "waiting for ns-3..."
wait $PipePID
