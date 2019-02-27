1. TP/PRO/BV-26 Router – Joins the network
Verify that the router device is capable to join a network.

Note: To make test work under NS ieee address differs from TC.

2. TP/PRO/BV-27 End Device Joins the network
Verify that the end device is capable to join a network and make a device announcement.

Note: To make test work under NS ieee address differs from TC.

3. TP/PRO/BV-28 Rejoin end device (Rx - Off)
Validate end device (with Rx on idle=false/true) does not change its short address
on rejoin, when it leaves from one parent and joins with another parent.

Note: To make test work under NS ieee address differs from TC. Also ED do not save it's
address after rejoin to the coordinator, because we do not support stochastic address
scheme and each coordinator and routes has it's own address space. But we could easily
add special define inside stack and save address just for this test.
