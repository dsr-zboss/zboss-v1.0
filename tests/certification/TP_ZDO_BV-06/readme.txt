Test case 14.12 TP/ZDO/BV-06 ZC-ZDO-Transmit Bind/Unbind_req
1) gZED1 and gZED2 join the PAN via ZDO functionality
2) Perform a BIND on gZED1. The BIND operation shall be between gZED1 and gZED2
3) Test Driver (Device Id 0x0000) on gZED1 issues Buffer Test Request (0x001C)
4) The DUT ZC issues UNBIND for gZED1
5) Test Driver (Device Id 0x0000) on gZED1 issues "Buffer Test Request" (0x001C)

To execute test run run.sh. Check log files on test finish.
tp_zdo_bv_06_gZED1*.log should contain "Test is finished, status OK"
message on success.
