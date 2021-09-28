################# Add include #################
# pjsip
list(APPEND ADD_INCLUDE "pjsip/pjproject/pjlib/include"
                        "pjsip/pjproject/pjlib-util/include"
                        "pjsip/pjproject/pjmedia/include"
                        "pjsip/pjproject/pjnath/include"
                        "pjsip/pjproject/pjsip/include"
                        "pjsip/inc")

list(APPEND ADD_PRIVATE_INCLUDE "")
###############################################

############## Add source files ###############
# list(APPEND ADD_SRCS  "src")
append_srcs_dir(ADD_SRCS "pjsip/src")
# list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
###############################################

###### Add required/dependent components ######
# list(APPEND ADD_REQUIREMENTS component1)
###############################################

############ Add static libs ##################
# list(APPEND ADD_STATIC_LIB "")
###############################################
if(NOT CONFIG_PJSIP_USER_DYNAMIC_LIB)

endif()
############ Add dynamic libs ##################
if(CONFIG_PJSIP_USER_DYNAMIC_LIB)
list(APPEND ADD_DYNAMIC_LIB "pjsip/pjproject/pjlib/lib/libpj.so"
                            "pjsip/pjproject/pjlib-util/lib/libpjlib-util.so"
                            "pjsip/pjproject/pjmedia/lib/libpjmedia-audiodev.so"
                            "pjsip/pjproject/pjmedia/lib/libpjmedia-codec.so"
                            "pjsip/pjproject/pjmedia/lib/libpjmedia.so"
                            "pjsip/pjproject/pjmedia/lib/libpjmedia-videodev.so"
                            "pjsip/pjproject/pjmedia/lib/libpjsdp.so"
                            "pjsip/pjproject/pjnath/lib/libpjnath.so"
                            "pjsip/pjproject/pjsip/lib/libpjsip-simple.so"
                            "pjsip/pjproject/pjsip/lib/libpjsip-ua.so"
                            "pjsip/pjproject/pjsip/lib/libpjsip.so"
                            "pjsip/pjproject/pjsip/lib/libpjsua.so"
                            "pjsip/pjproject/pjsip/lib/libpjsua2.so"
                            "pjsip/pjproject/third_party/lib/libg7221codec.so"
                            "pjsip/pjproject/third_party/lib/libgsmcodec.so"
                            "pjsip/pjproject/third_party/lib/libilbccodec.so"
                            "pjsip/pjproject/third_party/lib/libresample.so"
                            "pjsip/pjproject/third_party/lib/libspeex.so"
                            "pjsip/pjproject/third_party/lib/libsrtp.so"
                            "pjsip/pjproject/third_party/lib/libyuv.so")
endif()
###############################################

#### Add compile option for this component ####
#### Just for this component, won't affect other 
#### modules, including component that depend 
#### on this component
# list(APPEND ADD_DEFINITIONS_PRIVATE -DAAAAA=1)

#### Add compile option for this component
#### and components denpend on this component
# list(APPEND ADD_DEFINITIONS -DAAAAA222=1
#                             -DAAAAA333=1)
###############################################

# register component, DYNAMIC or SHARED flags will make component compiled to dynamic(shared) lib
register_component()

