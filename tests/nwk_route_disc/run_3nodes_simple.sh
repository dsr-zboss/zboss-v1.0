#!/bin/sh
#
#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
#
# Purpose: NWK route discovery test using ns-3 sim and native Linux executable
#

killch() {
    kill $ze1PID $router2PID $router1PID $coordPID $PipePID
}

killch_ex() {
    killch
    echo Interrupted by user!
    exit 1
}

trap killch_ex TERM INT

rm -f *.log *.pcap *.dump

PIPE_NAME=/tmp/zt`whoami`
echo "run ns-3"
../../devtools/network_simulator/network_simulator --nNode=3 --Join=1  --PipeName=${PIPE_NAME} --XGML=3nodes_simple.xgml&
PipePID=$!

sleep 5

echo "run coordinator"
./nwk_route_discovery_coordinator ${PIPE_NAME}0.write ${PIPE_NAME}0.read &
coordPID=$!

sleep 30

echo "run route discovery destination"
./nwk_route_discovery_destination ${PIPE_NAME}1.write ${PIPE_NAME}1.read &
dstPID=$!

sleep 30

echo "run route discovery source"
./nwk_route_discovery_source ${PIPE_NAME}2.write ${PIPE_NAME}2.read 1 &
srcPID=$!

sleep 60

echo "kill pids"
kill $srcPID
kill $dstPID
kill $coordPID
kill $PipePID

echo "waiting for source node..."
wait $srcPID
echo "waiting for destination node..."
wait $dstPID
echo "waiting for coordinator node..."
wait $coordPID
echo "waiting for ns-3..."
wait $PipePID

if grep "ROUTE DISCOVERY SUCCESS" route_disc_dst*.log
#if grep "ROUTE DISCOVERY SUCCESS" route_disc_src*.log
then
  echo "DONE. TEST PASSED!!!"
else
  echo "ERROR. TEST FAILED!!!"
fi

