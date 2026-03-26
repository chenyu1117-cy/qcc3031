#ifndef __MY_PS_H_
#define __MY_PS_H_

#include <ps.h>

uint16 psWrite(uint16 key, const void *data, uint32 bytes);
uint32 psRead(uint16 key, void *data, uint32 bytes);

#endif /* __MY_PS_H_ */