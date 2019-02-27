14.3TP/ZDO/BV-01: ZC-ZDO-Receive Device Discovery 
The DUT as ZigBee coordinator shall respond to a device discovery request from a remote 
node.
1 gZED1 ZDO issues NWK_addr_req (single response)
2 DUTZC answers with NWK_add_resp
3 gZED1 ZDO issues NWK_addr_req (extended response)
4 DUTZC answers with NWK_add_resp (extended)
5 gZED1 ZDO issues IEEE_addr_req to the DUT (single response)
6 DUTZC answers with IEEE_add_resp
7 gZED1 ZDO issues IEEE_addr_req to the DUT (extended response)
8 DUTZC answers with NWK_add_resp(extended)

Note, that router doesn't appear in PV, but it answers on addr_req's too.
And to send an answer on broadcast *addr_req it needs a route request.


AZD1 Mandatory Service and Device Discovery
AZD2 Mandatory attributes of Service and Device Discovery
ALF1 Support of APS-DATA.request,  APS-DATA.confirm
ALF2 Support of APS-DATA.indication
NLF1 Support of NLDE-DATA.request, NLDE-DATA.confirm
NLF2 Support of NLDE-Data.indication
