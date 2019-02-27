# kill previous stuff

rm -f *.log

#running network simulator

uniq_name=`whoami`
echo $uniq_name

echo "Using NS pipe template /tmp/zigbee_${uniq_name}_pipe"

../../../ns/ns-3.7/build/debug/examples/udp-pipe/udp-pipe -nNode=2 -PipeName="/tmp/ns_${uniq_name}_pipe" &
nsPID=$!

sleep 3

ps -ef | grep $nsPID



../../../stack/devtools/pipe_data_router/pipe_data_router -sim_port 11111 -ns3_rpipe "/tmp/ns_${uniq_name}_pipe0.read" -ns3_wpipe "/tmp/ns_${uniq_name}_pipe0.write" -sim_rpipe "/tmp/s51_${uniq_name}_pipe0.read" -sim_wpipe "/tmp/s51_${uniq_name}_pipe0.write" -trace "trace_node0.log" &
sleep 3
node0_pipe_router_PID=$!

../../../stack/devtools/pipe_data_router/pipe_data_router -sim_port 22222 -ns3_rpipe "/tmp/ns_${uniq_name}_pipe1.read" -ns3_wpipe "/tmp/ns_${uniq_name}_pipe1.write" -sim_rpipe "/tmp/s51_${uniq_name}_pipe1.read" -sim_wpipe "/tmp/s51_${uniq_name}_pipe1.write" -trace "trace_node1.log" &
sleep 3
node1_pipe_router_PID=$!


echo "r" | s51 nwk_data_test_first.hex -t 8052 -S in="/tmp/s51_${uniq_name}_pipe0.write",out="/tmp/s51_${uniq_name}_pipe0.read"  &
echo "r" | s51 nwk_data_test_second.hex -t 8052 -S in="/tmp/s51_${uniq_name}_pipe1.write",out="/tmp/s51_${uniq_name}_pipe1.read" &
sleep 3


node1_s51_PID=$!

#echo "wait for NS"
#wait $nsPID
#echo "wait for node0 pipe router"
#wait $node0_pipe_router_PID
#echo "wait for node0 s51"
#wait $node0_s51_PID
#echo "wait for node1 pipe router"
#wait $node1_pipe_router_PID
#echo "wait for node0 s51"
#wait $node1_s51_PID 

