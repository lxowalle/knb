################# Add include #################
list(APPEND ADD_INCLUDE "inc"
    )
# list(APPEND ADD_PRIVATE_INCLUDE "include_private")
###############################################

############## Add source files ###############
# list(APPEND ADD_SRCS  "")
# aux_source_directory(src ADD_SRCS)  
append_srcs_dir(ADD_SRCS "src")     
# list(REMOVE_ITEM COMPONENT_SRCS "src/test.c")
###############################################

###### Add required/dependent components ######
# list(APPEND ADD_REQUIREMENTS component1)
###############################################

############ Add static libs ##################
# list(APPEND ADD_STATIC_LIB "")
###############################################

############ Add dynamic libs ##################
# list(APPEND ADD_DYNAMIC_LIB "")
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

