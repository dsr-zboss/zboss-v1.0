Test idea is:
- create PRO network from ZC, ZR1, ZR2, security on
- join ZED (2007) to the network to ZR1, authenticate
- switch off zr1
- send Buffer Test req from zed to zr1
- rejoin zed to zr2
- send Buffer Test req

test is to be done with ZC, ZR1, ZR2 at Ember. We have 2 devices only.

First try with all devices running our stack.

Test setup for our stack:
- ZC - children cnt = 2. Security on. Assign NWK key. Start.
- ZR1 - join
	After ZED join complete switch off ZR1
- ZR2 - set children cnt 0. Join.
	After startup complete sleep 10 s, then set children cnt 1
- ZED - join (joins to ZR1)
	After startup complete (will be run twice):
		sleep 20s.
		sent Buffer Test to ZC

The idea is: ZED fails to send Buffer Test to ZC and initiates rejoin, joins to ZR2,
then test Buffer Test req again.


This test is not well debugged yet.


Got following reqyest from Claridi:

TUV said that we just need implement as follows:

1. DUTZR/ZED join to PRO network as end device
2. DUT send buffer_test_req to zc via zr1, and zc send buffer_test_rsp to DUT
via zr1.


So, need 2 alternate tests:
a) ZR joins PRO network (as ZED) and sends buffer test request. No security.
tp_pro_bv_13_13_18_alt_zr.c 
b) ZED joins PRO network and sends buffer test request. No security.
tp_pro_bv_13_13_18_alt_zed.c 