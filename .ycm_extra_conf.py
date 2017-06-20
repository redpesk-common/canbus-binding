def FlagsForFile( filename, **kwargs ):
  return {
    'flags': [ '-x', 'c++', '-Wall', '-Wextra', '-Werror', '-std=c++11',
'-DPB_FIELD_16BIT', '-D_REENTRANT', "-I/usr/include/json-c",
"-I/opt/include",
"-I/home/claneys/Workspace/Sources/IOTbzh/low-level-can-service/CAN-binder/libs/ini-config",
"-I/home/claneys/Workspace/Sources/IOTbzh/low-level-can-service/CAN-binder/libs/openxc-message-format/gen/cpp",
"-I/home/claneys/Workspace/Sources/IOTbzh/low-level-can-service/CAN-binder/libs/openxc-message-format/../nanopb",
"-I/home/claneys/Workspace/Sources/IOTbzh/low-level-can-service/CAN-binder/libs/uds-c/src",
"-I/home/claneys/Workspace/Sources/IOTbzh/low-level-can-service/CAN-binder/libs/isotp-c/src",
"-I/home/claneys/Workspace/Sources/IOTbzh/low-level-can-service/CAN-binder/libs/bitfield-c/src" ],
  }
