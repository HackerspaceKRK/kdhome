#include "provider_output.h"

#include "providers.h"
#include <kdhome.h>
#include "providers_settings.h"

#include <string.h>

#ifndef PROVIDER_DEBUG
#define PROVIDER_DEBUG(x,...)
#endif

void provOutputReset()
{
}
void provOutputRegister()
{
	PROVIDER_DEBUG("REGISTER OUTPUT\r\n");
	TProvOutputRegisterPacket p;
	provPrepareHeader((TProvHeader*)&p);
	p.header.type = PROVIDER_TYPE_OUTPUT;
	p.header.cmd = OUTPUT_CMD_REGISTER;
	p.cnt = OUTPUTS_COUNT;
	provSendPacket(&p, sizeof(p));
}
void provOutputProcess(const void* data, int len)
{
	TSrvHeader *header = (TSrvHeader*)data;
	
	switch (header->cmd)
	{
	case OUTPUT_CMD_SET_OUTPUTS:
	{
		TSrvOutputSetOutputsPacket *p = (TSrvOutputSetOutputsPacket*)data;
		int i;
		for (i = 0; i < p->cnt; i++)
		{
			int en = p->outputs & (1 << i);
			provOutputSetOutput(i, en);
		}
		provOutputUpdate();
	}
	break;
	}
}
void provOutputTmr()
{
}
