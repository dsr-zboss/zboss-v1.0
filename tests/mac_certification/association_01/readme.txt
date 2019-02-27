TP/154/MAC/ASSOCIATION-01
Check D.U.T. (as coordinator) correctly receives association request frame and generates positive response.


FFD DUT <--> RFD1

RFD1 sends Association request to FFD.

FFD DUT - test based on zdo_startup_zc (full build), but short address modified after startup to be 0x1122.


RFD1 test - association_01_rfd.c  - associate without discovery.