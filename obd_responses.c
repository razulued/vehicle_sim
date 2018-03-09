#include <stdio.h>
#include "obd_responses.h"
#include <string.h>

#define MODE_1 0x01
#define PID_0  0x00
#define PID_0C 0x0C
#define PID_0D 0x0D

const s_canframe mode1_pid0 	= { 0x7E8, 0x08, { 0x06, 0x41, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 } };
const s_canframe mode1_pid0C 	= { 0x7E8, 0x08, { 0x04, 0x41, 0x0C, 0x12, 0x34, 0x00, 0x00, 0x00 } };
const s_canframe mode1_pid0D 	= { 0x7E8, 0x08, { 0x03, 0x41, 0x0D, 0xA5, 0x00, 0x00, 0x00, 0x00 } };

int obd_response(struct can_frame *frame_in, s_canframe *frame_out)
{
	int ret = 0;
	uint8_t obd_mode;
	uint8_t obd_pid;
	if(0 == (frame_in->data[0] & 0xF0))
	{
//		printf("Single frame\n");
		obd_mode = frame_in->data[1];
		obd_pid = frame_in->data[2];
		if(MODE_1 == obd_mode)
		{
			switch(obd_pid)
			{
				case PID_0:
				memcpy(frame_out,&mode1_pid0, sizeof(s_canframe));
				ret = 1;	
				break;
				
				case PID_0C:
				memcpy(frame_out,&mode1_pid0C, sizeof(s_canframe));
				ret = 1;	
				break;
				
				case PID_0D:
				memcpy(frame_out,&mode1_pid0D, sizeof(s_canframe));
				ret = 1;	
				break;
			}
		}
	}
	
	return ret;
}


