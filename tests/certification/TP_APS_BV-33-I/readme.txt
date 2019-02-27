Test case 14.21TP/APS/ BV-33-I End Device Binding (HC V1 DUT ZC).
1) gZED1, gZED2 joins the network at DUT ZC
2) gZED1 and gZED2 perform End Device Bind
3) Transmit Test Buffer Request from gZED1
4) Transmit Test Buffer Request from gZED2 - skip this point,
   because reuest uses cluster id 0x1C and according to test
   it was not binded - it seems test case is incorrect here
5) gZED1 and gZED2 perform End Device Bind (for toggling)
6) Transmit Test Buffer Request from gZED1
7) Transmit Test Buffer Request from gZED2 - skip this point,
the same as (4)

To execute test run run.sh. Check log files on test finish.
tp_aps_bv_33_i_gZED1*.log should contain "Test is finished, status OK"
message on success.
