#!bin/bash

# in ascii:123456
# CRC8-ROHC             checksum=0x57
# CRC8                  checksum=0xFD
# CRC-16/IBM            checksum=0x29E4
# CRC-16/CCITT-FALSE    checksum=0x2EF4
# CRC32                 checksum=0x972D361
# CRC32/MPEG-2          checksum=0xD8F06C8F

@echo off

::设置CRC工具路径
CRC_TOOL_PATH=build\out\crc
::设置CRC校验输入的字符串
IN_DATA=123456

echo -----------------------------------
echo CRC8-ROHC
${CRC_TOOL_PATH} -l -w 8 -p 07 --init FF --xor 00 -s ${IN_DATA}

echo -----------------------------------
echo CRC-8
${CRC_TOOL_PATH} -m -w 8 -p 07 --init 00 --xor 00 -s ${IN_DATA}

echo -----------------------------------
echo CRC-16/IBM
${CRC_TOOL_PATH} -l -w 16 -p 8005 --init 0000 --xor 0000 -s ${IN_DATA}

echo -----------------------------------
echo CRC-16/CCITT-FALSE
${CRC_TOOL_PATH} -m -w 16 -p 1021 --init FFFF --xor 0000 -s ${IN_DATA}

echo -----------------------------------
echo CRC32
${CRC_TOOL_PATH} -l -w 32 -p 04C11DB7 --init FFFFFFFF --xor FFFFFFFF -s ${IN_DATA}

echo -----------------------------------
echo CRC32/MPEG-2
${CRC_TOOL_PATH} -m -w 32 -p 04C11DB7 --init FFFFFFFF --xor 00 -s ${IN_DATA}

pause