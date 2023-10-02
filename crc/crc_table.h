#ifndef __CRC_TABLE_H_
#define __CRC_TABLE_H_

#include "crc.h"

extern const uint8_t CRCTableMSB0x7[256];
extern const uint16_t CRCTableLSB0xa001[256];
extern const uint32_t CRCTableLSB0xedb88320[256];

#endif /*__CRC_TABLE_H_*/