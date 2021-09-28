#!/bin/bash

PJPROJECT_DST_DIR=/home/sipeed/lxowalle/knb/components/third_party/pjsip/pjproject
PJPROJECT_SRC_DIR=/home/sipeed/lxowalle/pjproject

rm -r ${PJPROJECT_DST_DIR}/pjlib/include
rm -r ${PJPROJECT_DST_DIR}/pjlib-util/include
rm -r ${PJPROJECT_DST_DIR}/pjmedia/include
rm -r ${PJPROJECT_DST_DIR}/pjnath/include
rm -r ${PJPROJECT_DST_DIR}/pjsip/include
# rm -r ${PJPROJECT_DST_DIR}/third_party/include

cp -r ${PJPROJECT_SRC_DIR}/pjlib/include ${PJPROJECT_DST_DIR}/pjlib/include
cp -r ${PJPROJECT_SRC_DIR}/pjlib-util/include ${PJPROJECT_DST_DIR}/pjlib-util/include
cp -r ${PJPROJECT_SRC_DIR}/pjmedia/include ${PJPROJECT_DST_DIR}/pjmedia/include
cp -r ${PJPROJECT_SRC_DIR}/pjnath/include ${PJPROJECT_DST_DIR}/pjnath/include
cp -r ${PJPROJECT_SRC_DIR}/pjsip/include ${PJPROJECT_DST_DIR}/pjsip/include
# cp -r ${PJPROJECT_SRC_DIR}/third_party/include ${PJPROJECT_DST_DIR}/third_party/include

rm -r ${PJPROJECT_DST_DIR}/pjlib/lib
rm -r ${PJPROJECT_DST_DIR}/pjlib-util/lib
rm -r ${PJPROJECT_DST_DIR}/pjmedia/lib
rm -r ${PJPROJECT_DST_DIR}/pjnath/lib
rm -r ${PJPROJECT_DST_DIR}/pjsip/lib
rm -r ${PJPROJECT_DST_DIR}/third_party/lib

cp -r ${PJPROJECT_SRC_DIR}/pjlib/lib ${PJPROJECT_DST_DIR}/pjlib/lib
cp -r ${PJPROJECT_SRC_DIR}/pjlib-util/lib ${PJPROJECT_DST_DIR}/pjlib-util/lib
cp -r ${PJPROJECT_SRC_DIR}/pjmedia/lib ${PJPROJECT_DST_DIR}/pjmedia/lib
cp -r ${PJPROJECT_SRC_DIR}/pjnath/lib ${PJPROJECT_DST_DIR}/pjnath/lib
cp -r ${PJPROJECT_SRC_DIR}/pjsip/lib ${PJPROJECT_DST_DIR}/pjsip/lib
cp -r ${PJPROJECT_SRC_DIR}/third_party/lib ${PJPROJECT_DST_DIR}/third_party/lib