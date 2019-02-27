Test for intra-PAN portability (security).
Test rejoin after security failure at child,

3 devices: ZC, ZR1, ZR2
ZC accepts 1 child only, ZR1 acept all children.

- ZR1 joins to ZC
- ZR2 joins to ZR1
- ZR2 changes key # in NIB (dirty hack)
- ZR1 sends pkt to ZR2 (butter test req)
- ZR2 fails to unsecure packet and rejoins to ZR1
- ZR1 sends buffer test req, ZR2 successfully answers.

Tu run test type
sh run.sh


