#!/bin/bash
PJPROJECT_DST_DIR=/home/liuxo/app/knb/components/third_party/FFmpeg
PJPROJECT_SRC_DIR=/home/liuxo/third_party/FFmpeg/install

if [ ! -d ${PJPROJECT_SRC_DIR} ];then
    echo "Error,${PJPROJECT_SRC_DIR} is not existed!"
fi

rm -rf ${PJPROJECT_DST_DIR}/include
rm -rf ${PJPROJECT_DST_DIR}/lib

cp ${PJPROJECT_SRC_DIR}/include ${PJPROJECT_DST_DIR} -r
mkdir ${PJPROJECT_DST_DIR}/lib
cp ${PJPROJECT_SRC_DIR}/lib/lib*.so* ${PJPROJECT_DST_DIR}/lib -rd

# Not need
rm ${PJPROJECT_DST_DIR}/lib/libavdevice*
rm ${PJPROJECT_DST_DIR}/lib/libavfilter*

if [ $? -ne 0 ];then
    echo "Error!"
    exit 1
fi
