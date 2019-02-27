#!/bin/sh
#
#/***************************************************************************
#*                                                                          *
#* INSERT COPYRIGHT HERE!                                                   *
#*                                                                          *
#****************************************************************************
#
# Purpose: trivial NWK data test using ns-3 sim and native Linux executable
#

export LD_LIBRARY_PATH=`pwd`/../../../ns/ns-3.7/build/debug


analyze_dump()
{
    if ../../devtools/dump_converter/dump_converter -ns $1 tmp.pcap
    then
        analyze_pcap tmp.pcap
        return $?
    fi
    return 1
}

analyze_pcap()
{
    n_nwk=`tshark -V -r $1 | grep 'ZigBee Network Layer' | wc -l`
    if [ $n_nwk -eq 2 ]
    then
        return 0
    else
        return 1
    fi
}

rm -f *.log *.pcap *.dump

../../devtools/network_simulator/network_simulator --nNode=2 >ns.txt 2>&1 &
PipePID=$!


./nwk_data_test_first /tmp/zudp0.write /tmp/zudp0.read &
firstPID=$!

sleep 1

./nwk_data_test_second /tmp/zudp1.write /tmp/zudp1.read & 
secondPID=$!

wait $firstPID
wait $secondPID

if grep "SUCCESS" nwk_test_first*.log
then
    if grep "SUCCESS" nwk_test_second*.log
    then
        wait $PipePID
        if analyze_pcap udp-pipe-0-1.pcap && analyze_pcap udp-pipe-1-1.pcap
        then
            if analyze_dump $firstPID.dump && analyze_dump $secondPID.dump
            then
                echo test nwk_data finished ok
            fi
        fi
    else
        grep "ERROR" nwk_test_second*.log
        kill $PipePID
    fi
else
    grep "ERROR" nwk_test_first*.log
    kill $PipePID
fi


