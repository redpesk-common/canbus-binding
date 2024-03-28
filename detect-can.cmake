# Activate J1939
# Need module in kernel
# --------------

#execute_process(COMMAND ls $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/j1939.h RESULT_VARIABLE result OUTPUT_QUIET ERROR_QUIET)

find_file(result j1939.h $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/)

if("${WITH_FEATURE_J1939}" STREQUAL "ON")
       if("${result}" STREQUAL "result-NOTFOUND")
               message(FATAL_ERROR "ERROR: No $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/j1939.h headers found")
       endif()
elseif("${WITH_FEATURE_J1939}" STREQUAL "OFF")
       set(result result-NOTFOUND)
endif()

if("${result}" STREQUAL "result-NOTFOUND")
       message("Feature J1939 disabled")
       set(WITH_FEATURE_J1939 OFF)
else()
       message("Feature J1939 enabled")
       set(WITH_FEATURE_J1939 ON)
       add_definitions(-DUSE_FEATURE_J1939)
endif()

# Activate ISO TP
# Need module in kernel
# --------------

find_file(result2 isotp.h $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can)

if("${WITH_FEATURE_ISOTP}" STREQUAL "ON")
       if("${result2}" STREQUAL "result2-NOTFOUND")
               message(FATAL_ERROR "ERROR: No $ENV{PKG_CONFIG_SYSROOT_DIR}/usr/include/linux/can/isotp.h headers found")
       endif()
elseif("${WITH_FEATURE_ISOTP}" STREQUAL "OFF")
       set(result2 result2-NOTFOUND)
endif()

if("${result2}" STREQUAL "result2-NOTFOUND")
       message("Feature ISO TP disabled")
       set(WITH_FEATURE_ISOTP OFF)
else()
       message("Feature ISO TP enabled")
       set(WITH_FEATURE_ISOTP ON)
       add_definitions(-DUSE_FEATURE_ISOTP)
endif()

