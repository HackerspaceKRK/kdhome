#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>

typedef struct
{
	struct pbuf* p;
	uint16_t pos;
} TByteBuffer;

int bufferFetch(TByteBuffer* buf, char* val, uint16_t len);
#define BYTEBUFFER_FETCH(buf,val) bufferFetch(buf,(char*)&(val),sizeof(val))

int bufferAppend(TByteBuffer* buf, char* val, uint16_t len);
#define BYTEBUFFER_APPEND(buf,val) bufferAppend(buf,(char*)&(val),sizeof(val))

#define BYTEBUFFER_LEN(buf) (buf->p->tot_len)

#endif
