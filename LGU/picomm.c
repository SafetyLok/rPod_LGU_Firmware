#include <picomm.h>
#include <ser.h>
#include <picomm_internal.h>

#define PICOMM_SERPORT_IDX 0

void picomm_onRxEvt(struct rI2CRX_decParam decParam);
int16_t picomm_hal_read(void);
void picomm_hal_tx(uint8_t);

void picomm_init()
{
    rI2CRX_recvDecParamCB = picomm_onRxEvt;
}

void picomm_task()
{
    int16_t c;
    #ifdef PICOMM_PROCESS_ALL
    while
    #else
    if
    #endif
    ((c = picomm_hal_read()) >= 0) {
        uint8_t x = (uint8_t)c;
        rI2CRX_receiveBytes(&x, 1);
    }
}

void picomm_onRxEvt(struct rI2CRX_decParam decParam)
{
	static uint64_t tmp;
	tmp = 0;
	memcpy((void*)&tmp, decParam.val, decParam.length);
    picomm_appHandler(decParam.index, decParam.type, decParam.length, (uint8_t*)&tmp);
}

void rI2CTX_add_checked_byte(uint8_t byte);
void rI2CTX_addHeader(uint8_t dataType, uint16_t index);

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

void picomm_tx(uint16_t idx, uint8_t type, uint8_t length, uint8_t* data)
{
    uint16_t i;

    rI2CTX_beginFrame();
    rI2CTX_addHeader(type, idx);
    for (i = 0; i < length; i++) {
        // because for some reason David decided that big endian is the hottest thing
        rI2CTX_add_checked_byte(data[length - i - 1]);
    }
    rI2CTX_endFrame();

    // send all of it
    for (i = 0; i < rI2CTX_frameLength; i++) {
        picomm_hal_tx(rI2CTX_buffer[i]);
    }
}

int16_t picomm_hal_read()
{
    return ser_read(&serport[PICOMM_SERPORT_IDX]);
}

void picomm_hal_tx(uint8_t x)
{
    ser_tx(&serport[PICOMM_SERPORT_IDX], x);
}
