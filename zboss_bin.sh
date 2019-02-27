#!/bin/bash

#Creates ZBOSS source archive for site.
#Be sure that Options, Platforms and links exist

rm Platform
rm Options

ln -s build-configurations/Options-linux-debug Options
ln -s build-configurations/Platform-linux Platform

echo REBUILD STACK
make rebuild

#echo REMOVING MAKEFILE LINKS AND TMP
#find . -type l -exec rm {} \;
bash remove_tmp.sh

echo REMOVING *.O
find . -name *.o -exec rm {} \;

tgt_fld=ZBOSS_v1.0_ns_x86_bin

echo RECREATE TARGET FOLDER
rm -rf $tgt_fld
mkdir -p $tgt_fld/lib
mkdir -p $tgt_fld/include

echo COPYING LIBS
find . -name *.a -exec cp {} $tgt_fld/lib/ \;

echo COPYING INCLUDES
find include -name *.h -exec cp {} $tgt_fld/include/ \;
find osif/include -name *.h -exec cp {} $tgt_fld/include/ \;

echo COPYING DEVTOOLS
cp --parents devtools/dump_converter/dump_converter $tgt_fld/
cp --parents devtools/network_simulator/network_simulator $tgt_fld/
cp --parents devtools/win_com_dump/win_com_dump.exe $tgt_fld/

echo COPYING TESTS
for ker in tests/zdo_startup tests/zdo_start_secur tests/zdo_mgmt_nwk tests/zdo_lqi \
    tests/zdo_mgmt_joining_time_duration tests/zdo_join_duration tests/zdo_intra_pan_portability \
    tests/zdo_addr tests/zdo_active_ep tests/Makefile \
    tests/aib_nib_pib_test tests/nwk_best_route \
    tests/aps_dup_reject tests/aps_group tests/nwk_leave tests/nwk_passive_ack tests/nwk_status \
    tests/orphan_scan tests/remove_device \
    tests/certification/TP_APS_BV-09 tests/certification/TP_APS_BV-12-I tests/certification/TP_APS_BV-27-I \
    tests/certification/TP_APS_BV-29-I tests/certification/TP_APS_BV-35-I tests/certification/TP_NWK_BV-12 \
    tests/certification/TP_PRO_BV-26 tests/certification/TP_PRO_BV-35 tests/certification/TP_PRO_BV-36 \
    tests/certification/Makefile ; do

    cp -a --parents $ker $tgt_fld
done

echo COPY CONFIGS
cp build-configurations/Options-linux-sdk $tgt_fld/Options
cp build-configurations/Platform-linux $tgt_fld/Platform
touch $tgt_fld/deps

#echo COPYING OTHER STUFF
#for file in *.ini deps Doxy* zboss_open_source.ew* *.lin *.html Makefile mkt lin_gen.sh README.build readme.txt *.uv* ; do
#    cp $file $tgt_fld
#done

echo COPYING DOC
cp -a ../doc $tgt_fld
cp readme_sdk.txt $tgt_fld/

echo PACKING...
rm -rf ${tgt_fld}.tar.gz
tar -cz $tgt_fld -f ${tgt_fld}.tar.gz

#echo RESTORE THE LINKS
#restore_links