
Test TP/154/MAC/SCANNING-04

Perform an active scan on several channels to check positive response when coordinator(s) present
(short address with beacon payload)

To pass test zigbee stack should be build with defined ZB_MAC_TESTING_MODE
Test sets mac_auto_request = 1 to store all beacons, all received beacons are reported
into log file in zb_mlme_scan_confirm(). If mac_auto_request = 0, beacons are analysed
in zb_mlme_beacon_notify_indication() on every beacon frame receive. Beacon payload is
written into dump in zb_mlme_beacon_notify_indication() also.

On test finish check tst_ze*.log, pan descriptor should agree with one described in
the test spec:

4. PANDescriptorList:
CoordAddrMode=0x2 
CoordPanId=0x1aaa
CoordAddr=0xbb00 
LogicalChannel=0x14
SuperframeSpec = 0x4FFF
GTSPermit=FALSE
LinkQuality=0x00-0xff
Timestamp
SecurityFailure=SUCCESS
SecurityLevel=0 [ KeyIdMode = NULL
KeySource=NULL
KeyIndex=0x00]

Beacon Payload: 0x12 0x34

