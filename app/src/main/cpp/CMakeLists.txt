cmake_minimum_required(VERSION 3.22.1)

project("djsdmsplayer")

# 设置路径
set(LIBDMS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# 添加包含目录
include_directories(${INCLUDE_DIR})

# 添加libdms库
add_library(lib_dms SHARED IMPORTED)
set_target_properties(lib_dms PROPERTIES
        IMPORTED_LOCATION ${LIBDMS_DIR}/${CMAKE_ANDROID_ARCH_ABI}/libdms.so
)

# 创建原生库
add_library(dms_jni SHARED
        dms_jni_wrapper.cpp
        dms_player.cpp
)

# 链接库
target_link_libraries(dms_jni
        android
        log
        lib_dms
)

find_library(log-lib log)