#!/bin/bash

echo REMOVING LOGS, DUMPS AND TMPS FROM TESTS
find . -name '*~' -exec rm {} \;
find . -name '*.dump' -exec rm {} \;
find . -name '*.pcap' -exec rm {} \;
find . -name '*.log' -exec rm {} \;
find . -name 'ns.txt' -exec rm {} \;