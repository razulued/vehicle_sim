#ifndef OBD_RESPONSES_H
#define OBD_RESPONSES_H

#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdint.h>

typedef struct can_frame s_canframe;

extern int obd_response(struct can_frame *frame_in, s_canframe *frame_out);

#endif
