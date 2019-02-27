This is analog for the test 13.20TP/PRO/BV-15 Authentication of joining devices (No pre-
configured key).

PRO TC supposes High security mode, 2007 utilises Standard security, so instead of using Master key,
SKKE, authentication, TC must just send NWK key to the joined device: directly or indirectly.
ZC == TC in our case.

Actions for Standard security:
- execute Formation on ZC, set max_children in NIB to 1, so only 1 connect to ZC is possible
- ZR1 joins ZC
- ZC transport NK key to ZR1 (unsecured) - ZR1 authenticated.
- ZR2 joins ZR1
- ZR2 authentication: 
  - ZR1 sends UPDATE-DEVICE.request to ZC
  - ZC sends TRANSPORT-KEY.request to ZR1
  - ZR1 sends TRANSPORT-KEY.request to ZR2
- start router at ZR2, or set max_children in NIB to 0 to disallow joins to it
- ZED1 joins ZR1
- ZED1 authentication: 
  - ZR1 sends UPDATE-DEVICE.request to ZC
  - ZR1 sends UPDATE-DEVICE.request to ZC
  - ZC sends TRANSPORT-KEY.request to ZR1
  - ZED1 polls ZR1
  - ZR1 sends TRANSPORT-KEY.request to ZED1
-ZC unicast data to ZR1


To run this test, type:
sh run.sh

After test complete analyze traffic dump files.
