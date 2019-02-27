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
../../devtools/network_simulator/network_simulator --nNode=5 --Join=1 --PipeName=${PIPE_NAME} --XGML=5nodes_2routes.xgml &
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

echo "run node"
./nwk_route_discovery ${PIPE_NAME}2.write ${PIPE_NAME}2.read &
nodePID=$!

sleep 40

echo "run child"
./nwk_route_discovery_no_router ${PIPE_NAME}3.write ${PIPE_NAME}3.read &
childPID=$!

sleep 40

echo "run route discovery source"
./nwk_route_discovery_source ${PIPE_NAME}4.write ${PIPE_NAME}4.read 1 &
srcPID=$!

sleep 90

echo "kill pids"
kill $childPID
kill $nodePID
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
echo "waiting for node2..."
wait $childPID
echo "waiting for node3..."
wait $nodePID
echo "waiting for ns-3..."
wait $PipePID

if grep "ROUTE DISCOVERY SUCCESS" route_disc_dst*.log
then
  echo "DONE. TEST PASSED!!!"
else
  echo "ERROR. TEST FAILED!!!"
fi

