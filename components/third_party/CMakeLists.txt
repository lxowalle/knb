################# Add include #################
list(APPEND ADD_INCLUDE         "sqlite3" "/usr/include")

###############################################

############## Add source files ###############
list(APPEND ADD_SRCS            "sqlite3/sqlite3.c")
list(APPEND ADD_DYNAMIC_LIB "/usr/lib/x86_64-linux-gnu/libpthread.so"
                            "/usr/lib/x86_64-linux-gnu/libdl.so")
# list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
###############################################

###### Add required/dependent components ######
# list(APPEND ADD_REQUIREMENTS component1)
###############################################

############ Add static libs ##################
# list(APPEND ADD_STATIC_LIB "")
###############################################


############ Add dynamic libs ##################
if(CONFIG_PJSIP_ENABLE)
list(APPEND ADD_INCLUDE         "pjsip/pjproject/include"
                                "pjsip/inc")
list(APPEND ADD_PRIVATE_INCLUDE "pjsip/inc_private")
append_srcs_dir(ADD_SRCS        "pjsip/src")

if(CONFIG_PJSIP_USE_DYNAMIC_LIB)
list(APPEND ADD_DYNAMIC_LIB "pjsip/pjproject/lib/libpj.so.2"
                            "pjsip/pjproject/lib/libpjlib-util.so.2"
                            "pjsip/pjproject/lib/libpjmedia-audiodev.so.2"
                            "pjsip/pjproject/lib/libpjmedia-codec.so.2"
                            "pjsip/pjproject/lib/libpjmedia.so.2"
                            "pjsip/pjproject/lib/libpjmedia-videodev.so.2"
                            # "pjsip/pjproject/lib/libpjsdp.so"
                            "pjsip/pjproject/lib/libpjnath.so.2"
                            "pjsip/pjproject/lib/libpjsip-simple.so.2"
                            "pjsip/pjproject/lib/libpjsip-ua.so.2"
                            "pjsip/pjproject/lib/libpjsip.so.2"
                            "pjsip/pjproject/lib/libpjsua.so.2"
                            # "pjsip/pjproject/lib/libpjsua2.so"
                            "pjsip/pjproject/lib/libg7221codec.so.2"
                            "pjsip/pjproject/lib/libgsmcodec.so.2"
                            "pjsip/pjproject/lib/libilbccodec.so.2"
                            "pjsip/pjproject/lib/libresample.so.2"
                            "pjsip/pjproject/lib/libspeex.so.2"
                            "pjsip/pjproject/lib/libsrtp.so.2"
                            # "pjsip/pjproject/lib/libyuv.so"
                            )
endif()
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

