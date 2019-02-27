Test for REQUEST-KEY (4.4.6).
To run this test type 
$ sh run.sh

2 devices: ZC, ZR1

Test steps:
- ZR1 joins ZC
- after timeout ZR1 sends REQUEST-KEY ro ZC
- ZC sends active network key.

Not sure what is superior meaning of this test and at all REQUEST-KEY functionality
in absence of application keys.
But, according to document 08006r03, REQUEST-KEY is mandatory while application
keys are Optional, so we did not implement it.
