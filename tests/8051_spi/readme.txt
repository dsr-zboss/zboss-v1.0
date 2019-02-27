
SPI tests now works only with Keil environment. Keil debug script debug.ini is used 
for testing SPI. This script receives and sends data over SPI. Received bytes are 
written into internal memory starting with address X:0x700000.
How to use: 
- Compile test
- Start simulator
- In the memory dump window open adress X:0x700000
- Start test
- Messages in the memory dump appear:
Start SPI test
0) received: [<some latin letters>]
....
9) received: [<some latin letters>]
SPI test finished
