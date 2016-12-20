#ifndef _PICOMM_H_
#define _PICOMM_H_

#include <stdint.h>
#include <pi_comm/RX/rI2CRX.h>

#define PICOMM_TX_U8(i, x)     picomm_tx((i), rI2C_UINT8,  sizeof(uint8_t),  (uint8_t*)&(x))
#define PICOMM_TX_S8(i, x)     picomm_tx((i), rI2C_INT8,   sizeof(int8_t),   (uint8_t*)&(x))
#define PICOMM_TX_U16(i, x)    picomm_tx((i), rI2C_UINT16, sizeof(uint16_t), (uint8_t*)&(x))
#define PICOMM_TX_S16(i, x)    picomm_tx((i), rI2C_INT16,  sizeof(int16_t),  (uint8_t*)&(x))
#define PICOMM_TX_U32(i, x)    picomm_tx((i), rI2C_UINT32, sizeof(uint32_t), (uint8_t*)&(x))
#define PICOMM_TX_S32(i, x)    picomm_tx((i), rI2C_INT32,  sizeof(int32_t),  (uint8_t*)&(x))
#define PICOMM_TX_U64(i, x)    picomm_tx((i), rI2C_UINT64, sizeof(uint64_t), (uint8_t*)&(x))
#define PICOMM_TX_S64(i, x)    picomm_tx((i), rI2C_INT64,  sizeof(int64_t),  (uint8_t*)&(x))
#define PICOMM_TX_FLOAT(i, x)  picomm_tx((i), rI2C_FLOAT,  sizeof(float),    (uint8_t*)&(x))
#define PICOMM_TX_DOUBLE(i, x) picomm_tx((i), rI2C_DOUBLE, sizeof(double),   (uint8_t*)&(x))

void picomm_init(void);
void picomm_task(void);
void picomm_tx(uint16_t idx, uint8_t type, uint8_t length, uint8_t* data);

extern void picomm_appHandler(uint16_t idx, uint8_t type, uint8_t length, uint8_t* data);

#endif
