13.39TP/PRO/BV-38 Frequency Agility - Channel Changeover - ZR

Verify that the DUT acting as a ZR behaves as appropriate during channel
changeover

- gZC (Network Channel Manager) issues Mgmt_NWK_Update_req with channel mask for
channel m (Different than the one network is on), where n=present channel.
- After expiry of nwkNetworkBroadcastDeliveryTime gZC shall unicast a Buffer Test Request
- DUT ZR1 shall unicast a Buffer Test Response to gZC

To execute test start run.sh, to check results analyse dump files. On success test scripts
prints "DONE. TEST PASSED!!!" and "ERROR. TEST FAILED!!!" otherwise

