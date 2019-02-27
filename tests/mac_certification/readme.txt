mac_scanning tests

scanning_03 test is debugged on the device, everything is Ok, except one thing - coordinator always 
sends beacon on channel 16, i couldn't change it
scanning_04, 05 are compiled but not debugged, it is bery close to test 03, only differences in 
constant values on coordinator side
scanning_06 - need to start several coordinators (minimum 3), eac should be built with it's unique
value of ZB_TEST_COORD_NUMBER macro

Client side is the same for all the scanning tests, but i commited C-file for client into all 
tests, just in case
