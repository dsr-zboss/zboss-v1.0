#!/bin/bash

#Creates ZBOSS network simulator source archive for site.

tgt_fld=ZBOSS_ns_bin
ns=network_simulator
dc=dump_converter

echo BUILD STACK + NS + TESTS
ln -s build-configurations/Options-linux-debug Options
ln -s build-configurations/Platform-linux Platform

make makefile_links
make

bash remove_tmp.sh

echo RECREATE TARGET FOLDER
rm -rf $tgt_fld
mkdir $tgt_fld
mkdir $tgt_fld/${ns}
mkdir $tgt_fld/${dc}
mkdir $tgt_fld/doc
mkdir $tgt_fld/example

echo COPY SIMULATOR BINS...
cp devtools/${ns}/${ns} ${tgt_fld}/${ns}

echo COPY DUMP CONVERTER
cp devtools/${dc}/${dc} ${tgt_fld}/${dc}

echo COPY EXAMPLE
tst=tests/zdo_startup
find $tst -type f -executable -exec cp {} ${tgt_fld}/example \;
cp ${tst}/run*.sh ${tgt_fld}/example
cp ${tst}/readme.txt ${tgt_fld}/example
sed -i "s|../../devtools/|../|" ${tgt_fld}/example/run*.sh 


#for tst in tests/zdo_startup tests/zdo_start_secur tests/zdo_mgmt_nwk tests/zdo_lqi \
#    tests/zdo_mgmt_joining_time_duration tests/zdo_join_duration tests/zdo_intra_pan_portability \
#    tests/zdo_addr tests/zdo_active_ep ; do
#    
#    mkdir ${tgt_fld}/${tst}
#    find $tst -type f -executable -exec cp {} ${tgt_fld}/${tst} \;
#    cp ${tst}/run*.sh ${tgt_fld}/${tst}
#    cp ${tst}/readme.txt ${tgt_fld}/${tst}
#    sed -i "s|../../devtools/|../../|" ${tgt_fld}/${tst}/run*.sh 
#done

echo PREPARING THE DOCS
mkdir tmp
cd tmp
svn co svn://svn.lx-ltd.ru/zigbee/trunk/doc/customers/dsr/open_source/ns
cp ns/ZBOSS_NS_User_Manual.pdf ../${tgt_fld}/doc
cd ../
rm -rf tmp

echo PACKING...
rm ${tgt_fld}.tar.gz
tar -cz $tgt_fld -f ${tgt_fld}.tar.gz