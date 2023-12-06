cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_SYSTEM_NAME Generic)

project(crc C)

#静态库名
set(CRC_LIB_NAME crc)

#设置测试的可执行文件名
set(TEST_EXE_NAME crc_test)

#是否开启更详细的编译过程信息显示
SET(CMAKE_VERBOSE_MAKEFILE ON)

# 彩色日志输出；
set(CMAKE_COLOR_DIAGNOSTICS TRUE)

#设置生成目标的目录
set(EXECUTABLE_OUTPUT_PATH output)

#生成静态库
add_library(${CRC_LIB_NAME} STATIC)
#测试
add_executable(${TEST_EXE_NAME})

add_subdirectory(crc)

#全局宏
target_compile_definitions(${TEST_EXE_NAME}
    PRIVATE
    ${CMAKE_HOST_SYSTEM_NAME}
)

#源文件
target_sources(${TEST_EXE_NAME}
    PRIVATE
    main.c
    crc/crc_table.c 
)

#编译参数
target_compile_options(${TEST_EXE_NAME}
    PRIVATE
    -std=c99 -Wall -O0 -g
)

#链接静态库
target_link_libraries(${TEST_EXE_NAME}
    ${CRC_LIB_NAME}
)