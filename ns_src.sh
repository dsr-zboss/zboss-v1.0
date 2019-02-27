#!/bin/bash

#Creates ZBOSS network simulator source archive for site.

tgt_fld=ZBOSS_ns_src

bash remove_tmp.sh

echo RECREATE TARGET FOLDER
rm -rf $tgt_fld
mkdir -p $tgt_fld/doc
mkdir -p $tgt_fld/ns

mkdir -p $tgt_fld/dump_converter
mkdir -p $tgt_fld/dump_converter/include

echo COPY SOURCES...
echo NETWORK SIMULATOR
cp devtools/network_simulator/*.cpp $tgt_fld/ns
cp devtools/network_simulator/*.h $tgt_fld/ns
cp devtools/network_simulator/MakefileSeparate $tgt_fld/ns/Makefile
echo DUMP CONVERTER
cp devtools/dump_converter/*.c $tgt_fld/dump_converter
cp devtools/dump_converter/*.c $tgt_fld/dump_converter
cp devtools/dump_converter/MakefileSeparate $tgt_fld/dump_converter/Makefile
cp include/* $tgt_fld/dump_converter/include
cp osif/include/* $tgt_fld/dump_converter/include



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