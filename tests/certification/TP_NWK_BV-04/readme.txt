14.2TP/NWK/BV-04 ZR-ZDO-APL RX Join/Leave
Verify that the device ZDO Network Manager functionality is capable of a network join or
leave, and verify the status.

- DUT ZR performs a NWK LEAVE (self leave, address=NULL,
rejoin=FALSE)
- Based on existing policy, DUT ZR performs NWK Join.
- gZC performs a LEAVE, where NLMELEAVE.
request(DeviceAddress=IEEE of ZR, rejoin=FALSE;
remove children=FALSE);

to execute test start ru.sh
To check results, analyse log files, on success tp_nwk_bv_04_ZR(pid).log contains
"Device STARTED OK" after leave.
