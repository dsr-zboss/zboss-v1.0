zdo_zc_test_desc starts as ZC, zdo_zr_test_desc joins to it, then test sends
descriptors requests

To start tests in Linux / ns-3:
- setup ns-3 as described in doc/ns-3/README, cd ns/ns-3.7/build/debug/examples/udp-pipe,
run ./udp-pipe --nNode=2 --PipeName=/tmp/zzz --Join=1
- at another tty run ./zdo_zc_test_desc /tmp/zzz0.write /tmp/zzz0.read
- wait for Formation complete: seek for "Device STARTED OK" in the trace
- at another tty run ./zdo_zr_test_desc /tmp/zzz1.write /tmp/zzz1.read

Router (zdo_zr_test_desc) sends descriptor requests to coordinator.
Coordinator, in response sends descriptor.
Test results can be checked by analysing router log file zdo_zrXXXXX.log.
On test finish message "zdo descriptor test finished" is written with test status Ok or Failed.
