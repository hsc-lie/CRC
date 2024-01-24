#ifndef __CRC_H_
#define __CRC_H_

#ifndef NULL
#define NULL ((void *)(0))
#endif

typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int      uint32_t;

typedef enum
{
    CRC_FIRST_BIT_LSB,
    CRC_FIRST_BIT_MSB
}CRC_FIRST_BIT_t;

typedef struct
{
    CRC_FIRST_BIT_t FirstBit;  //字节顺序,LSB与相当于Refin=true,Refin=true,MSB相当于Refin=false,Refin=false
    uint8_t BitWide;           //位宽(例如8,16,32)
    uint32_t Poly;             //多项式的值
    uint32_t Checksum;         //CRC校验的值(未经过异或)
    void * CRCTable;           //CRC表的指针
}CRC_t;

extern void CRCInit(CRC_t *crc, CRC_FIRST_BIT_t firstBit, uint8_t bitWide, uint32_t poly);
extern void CRCGenerateTable(CRC_t *crc, void *table);
extern void CRCSetTable(CRC_t *crc, uint8_t *table);
extern void CRCStart(CRC_t *crc, uint32_t initValue);
extern void CRCUpdate(CRC_t *crc, uint8_t *data, uint32_t len);
extern uint32_t CRCGetChecksum(CRC_t *crc, uint32_t xorOutValue);

#endif /*__CRC_H_*/
 
