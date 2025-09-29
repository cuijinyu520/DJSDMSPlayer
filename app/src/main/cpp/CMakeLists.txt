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




#[[
    CMakeLists.txt 在 app/src/main/cpp/ 目录，
    1、在 Android Studio 中点击 Build > Make Project（或 Ctrl+F9），CMake 会根据配置编译 libdmstest 可执行文件
    2、编译产物位于： app/build/intermediates/cxx/Debug/<架构>/obj/<架构>/libdmstest
    3、推送可执行文件到设备通过 adb 命令将编译好的 libdmstest 推送到 Android 设备的可执行目录（如 /data/local/tmp/）：
       adb push app/build/intermediates/cxx/Debug/arm64-v8a/obj/arm64-v8a/libdmstest /data/local/tmp/
    4、推送测试文件（KDM 和 DCP）将 KDM 文件和 DCP 文件夹推送到设备中与代码中一致的路径（例如 /sdcard/dms_test/）：
       adb push /本地/KDM文件.xml /sdcard/dms_test/kdm/
       adb push /本地/DCP文件夹/ /sdcard/dms_test/dcp/
    5、赋予执行权限并运行
     进入设备 shell
       adb shell
    # 切换到可执行文件目录
       cd /data/local/tmp/
    # 赋予执行权限
       chmod 755 libdmstest
    # 运行测试程序
       ./libdmstest
]]
add_executable(libdmstest
        demo/libdmstest.cpp  # 指向 libdmstest.cpp 的路径
)
target_link_libraries(libdmstest
        log
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libdms.so  # 链接 libdms.so
)

