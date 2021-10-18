#!/bin/bash

PJSIP_PATH=../../components/third_party/pjsip
PJSIP_LIB_PATH=${PJSIP_PATH}/pjproject/lib

export LD_LIBRARY_PATH=${PJSIP_LIB_PATH}:${LD_LIBRARY_PATH}

echo $LD_LIBRARY_PATH

CURR_DIR=`pwd`
cd $CURR_DIR/build && make -j8 && cd $CURR_DIR
./build/pjsip_test
