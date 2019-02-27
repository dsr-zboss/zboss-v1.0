
14.9 TP/ZDO/BV-03: ZC-ZDO-Receive Service Discovery
The DUT as ZigBee coordinator shall respond to mandatory service discovery requests
from a remote node.

Test:
- gZED1 ZDO issues Node_Desc_req to parent ZDO
- gZED1 ZDO issues Power_Desc_req to the DUT
- gZED1 ZDO issues Simple_Desc_req to the DUT
- gZED1 ZDO issues Active_EP_req to the DUT
- gZED1 ZDO issues Match_Descr_req to the DUT

To execute test start run.sh and check logs for result

