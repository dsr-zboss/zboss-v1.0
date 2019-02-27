
13.38TP/PRO/BV-37 Frequency Agility - Receipt of unsolicited
Mgmt_NWK_Update_notify

Verify that the DUT acting as a Network Channel Manager correctly handles the
receipt of Mgmt_NWK_Update_notify commands and carries out a channel
changeover as appropriate.

Test procedure:
1) DUT ZR1 shall attempt to unicast 21 counted packets to gZR2
2) Disturber device is setup to ensure a greater than 25% transmit failure rate
3) DUT ZR1 shall APS unicast a Mgmt_NWK_Update_notify command to
the Network Channel Manager (gZC)

To execute test start run.sh, to check results analyse dump files


