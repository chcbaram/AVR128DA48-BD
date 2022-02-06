#ifndef SRC_COMMON_CORE_QBLOCK_H_
#define SRC_COMMON_CORE_QBLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "def.h"



typedef struct
{
  uint32_t in;
  uint32_t out;
  uint32_t len;
  uint32_t size;

  uint8_t *p_buf;
} qblock_t;


void     qblockInit(void);
bool     qblockCreate(qblock_t *p_node, uint8_t *p_buf, uint32_t length);
bool     qblockCreateBySize(qblock_t *p_node, uint8_t *p_buf, uint32_t size, uint32_t length);
bool     qblockWrite(qblock_t *p_node, uint8_t *p_data, uint32_t length);
bool     qblockRead(qblock_t *p_node, uint8_t *p_data, uint32_t length);
uint8_t *qblockPeekWrite(qblock_t *p_node);
uint8_t *qblockPeekRead(qblock_t *p_node);
uint32_t qblockAvailable(qblock_t *p_node);
void     qblockFlush(qblock_t *p_node);



#ifdef __cplusplus
}
#endif

#endif