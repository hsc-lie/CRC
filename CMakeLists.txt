cmake_minimum_required(VERSION 3.28.0)

project(crc_test C)

add_executable(${PROJECT_NAME})

#是否开启更详细的编译过程信息显示
SET(CMAKE_VERBOSE_MAKEFILE ON)

# 彩色日志输出；
set(CMAKE_COLOR_DIAGNOSTICS TRUE)

#设置生成目标的目录
set(EXECUTABLE_OUTPUT_PATH out)

#FALSE:Release
#TRUE:DEBUG
set(DEBUG TRUE)

#C标准
set(C_STD -std=c99)

#优化等级
if(${DEBUG})
    set(OPT -O0 -g) 
else()
    set(OPT -O3)
endif()

#全局宏
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_HOST_SYSTEM_NAME}
)

#头文件路径
target_include_directories(${PROJECT_NAME}
    PRIVATE
    crc
)

#源文件
target_sources(${PROJECT_NAME}
    PRIVATE
    main.c
    crc/crc.c
    crc/crc_table.c
)

#编译参数
target_compile_options(${PROJECT_NAME}
    PRIVATE
    ${C_STD}
    ${OPT}
    -Wall
)
