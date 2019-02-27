#!/bin/bash

#Packs sniffer binaries for Open source site for both Win and Unix

tgt_fld=ZBOSS_sniffer_v1.0_x86_$1_bin
sf=sniffer

bash remove_tmp.sh

echo RECREATE TARGET FOLDER
rm -rf $tgt_fld
mkdir -p ${tgt_fld}/gui
mkdir -p ${tgt_fld}/hex
mkdir -p ${tgt_fld}/doc

echo COPY SNIFFER BINARIES
cp -a devtools/${sf}/gui/exe/$1/* ${tgt_fld}/gui

echo COPY IMAGES
cp -a out/Smartrf05Eb-Release ${tgt_fld}/hex
cp -a out/CC2531-USB-Dongle-Release ${tgt_fld}/hex

echo COPY DOCS
mkdir tmp
cd tmp
svn co svn://svn.lx-ltd.ru/zigbee/trunk/doc/customers/dsr/open_source/sniffer
cp sniffer/ZBOSS_Sniffer_User_Manual.pdf ../${tgt_fld}/doc
cd ../
rm -rf tmp

echo PACKING...
rm ${tgt_fld}.tar.gz
tar -cz $tgt_fld -f ${tgt_fld}.tar.gz
