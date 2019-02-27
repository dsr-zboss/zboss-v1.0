13.29TP/PRO/BV-23 PanID conflict detection - ZR

4 devices: ZC1, ZR2, ZC2, ZR3.
No security.
Visibility:
ZC1 - ZR2 - ZC2 - ZR3.
- ZC1 starts.
- ZR2 joins it.
- ZC2 starts choosing panid as usual.
As a side effect is remembers ZC1's panid in the external variable
(special hack for this test).
- After start ZC2 changes PANID to be equal to ZC1's panid and updates beacon payload
- ZR3 tries to join and issues beacon request.
ZR3 will fail to join bacause of NS limitations (NS supports onlt 1 ZC), but it is not
problem in this test
- ZR2 sees ZC2's beacon and initiates PANID conflict resolution.
