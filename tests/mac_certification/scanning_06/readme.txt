
Test TP/154/MAC/SCANNING-06

Perform an active scan on several channels to check positive response when multiple coordinators present
(test for memory / communications overflow)


To pass test zigbee stack should be build with defined ZB_MAC_TESTING_MODE
Test sets mac_auto_request = 1 to store all beacons, all received beacons are reported
into log file in zb_mlme_scan_confirm(). If mac_auto_request = 0, beacons are analysed
in zb_mlme_beacon_notify_indication() on every beacon frame receive.
N coordinators send beacons (N = 3), N > ZB_ACTIVE_SCAN_MAX_PAN_DESC_COUNT. 

IMPORTANT! This test is not debugged on NS environment - can't create more then 1 coordinator.

On test finish check tst_ze*.log, pan descriptor should agree with one described in
the test spec:

4. PANDescriptorList:
CoordAddrMode=0x2
CoordPanId=0x1aaa
CoordAddr=0x000n (short address of detected FFD)
LogicalChannel=0x14
SuperframeSpec = 0x4FFF
GTSPermit=FALSE
LinkQuality=0x00-0xff
Timestamp
SecurityFailure=SUCCESS
SecurityLevel=0 [ KeyIdMode = NULL
KeySource=NULL
KeyIndex=0x00]
