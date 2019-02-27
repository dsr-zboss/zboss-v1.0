#!/bin/bash

#Creates ZBOSS source archive for site.
#Be sure that Options, Platforms and links exist

restore_links() {
    ln -s build-configurations/Options-linux-debug Options
    ln -s build-configurations/Platform-linux Platform
    make makefile_links
}

echo RESTORE THE LINKS
restore_links

echo CLEAN
make clean

echo REMOVING MAKEFILE LINKS AND TMP
find . -type l -exec rm {} \;
bash remove_tmp.sh

tgt_fld=ZBOSS_v1.0_src

echo RECREATE TARGET FOLDER
rm -rf $tgt_fld
mkdir $tgt_fld

echo COPYING KERNEL...
for ker in aps build-configurations common include mac nwk osif secur zdo \
    devtools/network_simulator devtools/dump_converter devtools/win_com_dump devtools/Makefile \
    tests/zdo_startup tests/zdo_start_secur tests/zdo_mgmt_nwk tests/zdo_lqi \
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

echo COPYING OTHER STUFF
for file in *.ini deps Doxy* zboss_open_source.ew* *.lin *.html Makefile lin_gen.sh README.build readme.txt *.uv* ; do
    cp $file $tgt_fld
done

echo COPYING DOC
cp -a ../doc $tgt_fld

echo PACKING...
rm -rf ${tgt_fld}.tar.gz
tar -cz $tgt_fld -f ${tgt_fld}.tar.gz