#!/bin/bash

PJPROJECT_DST_DIR=/home/liuxo/knb/components/third_party/pjsip/pjproject
PJPROJECT_SRC_DIR=/home/liuxo/third_party/pjproject/install

if [ ! -d ${PJPROJECT_SRC_DIR} ];then
    echo "Error,${PJPROJECT_SRC_DIR} is not existed!"
fi

rm -rf ${PJPROJECT_DST_DIR}
mkdir ${PJPROJECT_DST_DIR}

cp ${PJPROJECT_SRC_DIR}/include ${PJPROJECT_DST_DIR} -r
mkdir ${PJPROJECT_DST_DIR}/lib
cp ${PJPROJECT_SRC_DIR}/lib/lib*.so* ${PJPROJECT_DST_DIR}/lib

# pjsua2 is too big,delete its
rm ${PJPROJECT_DST_DIR}/lib/libpjsua2*

# we need not libyuv and libwebrtc,delete its
# rm ${PJPROJECT_DST_DIR}/lib/libyuv*
# rm ${PJPROJECT_DST_DIR}/lib/libwebrtc*

if [ $? -ne 0 ];then
    echo "Error!"
    exit 1
fi
