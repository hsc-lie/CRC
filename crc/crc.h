#ifndef __CRC_H_
#define __CRC_H_

#ifndef TRUE
#define TRUE (1) 
#endif

#ifndef FALSE
#define FALSE (!(TRUE))
#endif

#ifndef NULL
#define NULL ((void *)(0))
#endif

typedef unsigned char     bool;
typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int      uint32_t;


typedef struct
{
    bool IsLSB;
    uint8_t BitWide;
    uint32_t Poly;
    uint32_t CRCValue;
    const void * CRCTable;
}CRC_t;



extern void CRCInit(CRC_t * crc, uint8_t bitWide, uint32_t poly, bool isLSB);
extern void CRCGenerateTable(CRC_t * crc, void * table);
extern void CRCSetTable(CRC_t * crc, uint8_t * table);
extern void CRCStart(CRC_t * crc, uint32_t initValue);
extern void CRCUpdate(CRC_t * crc, uint8_t * data, uint32_t len);
extern uint32_t CRCGetCheckValue(CRC_t * crc, uint32_t xorOutValue);

#endif /*__CRC_H_*/
 
