#!/bin/bash

PJSIP_PATH=../../components/third_party/pjsip
PJSIP_LIB_PATH=${PJSIP_PATH}/pjproject/pjlib/lib:${PJSIP_PATH}/pjproject/pjlib-util/lib:${PJSIP_PATH}/pjproject/pjmedia/lib:${PJSIP_PATH}/pjproject/pjnath/lib:${PJSIP_PATH}/pjproject/pjsip/lib:${PJSIP_PATH}/pjproject/third_party/lib

export LD_LIBRARY_PATH=${PJSIP_LIB_PATH}:${LD_LIBRARY_PATH}

echo $LD_LIBRARY_PATH

CURR_DIR=`pwd`
cd $CURR_DIR/build && make -j8 && cd $CURR_DIR
./build/pjsip_test
