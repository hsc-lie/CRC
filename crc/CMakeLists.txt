target_sources(${CRC_LIB_NAME}
    PRIVATE
    crc.c
)

target_include_directories(${CRC_LIB_NAME}
    PUBLIC
    .
)

target_compile_options(${CRC_LIB_NAME}
    PRIVATE
    -std=c99 -O3 -Wall
)