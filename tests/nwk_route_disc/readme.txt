NOTE: route discovery tests doesn't work properly now, we still debugging it!

  To run test just run one of the script files (test.sh, test2.sh ...).
Check script content to see what xgml file it use and how many Zigbee stacks run.
To see the network topology just load xgml file into Jed editor. 0 - node is
always a coordinator.
  All of these tests do the same, they organized Zigbee network with depth > 1 and
transfer one packet with route discovery from the source to the destination node
through transit nodes. Test result will be written in console (success or fail).
  It's also possible to see traffic dump in Wireshark. To do it, convert .dump
file with dump_converter (stack/devtools/dump_converter) to pcap format and load
it to Wireshark with zudp plugin (ns/wireshark-plugin/zudp) installed.
Dump converter usage:
     ./dump_converter -ns <input>.dump <output>.pcap
