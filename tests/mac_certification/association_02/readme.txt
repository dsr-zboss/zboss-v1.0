TP/154/MAC/ASSOCIATION-02
Check D.U.T. (as coordinator) correctly receives association request frame and generates negative response (PanAtCapacity, PanAccessDenied).


FFD DUT <--> RFD1

RFD1 sends Association request to FFD.
FFD has ZG->nwk.nib.ed_child_num == ZG->nwk.nib.router_child_num == 0,
so it sends Association Response with status = MAC_PAN_AT_CAPACITY.

FFD DUT - test based on zdo_startup_zc (full build), but short address modified after startup to be 0x1122, 
ZG->nwk.nib.ed_child_num == ZG->nwk.nib.router_child_num set to 0 after init.

RFD1 test - association_02_rfd.c  - associate without discovery.