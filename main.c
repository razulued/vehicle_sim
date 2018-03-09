/*
 * untitled.c
 * 
 * Copyright 2015  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "obd_responses.h"


int soc;
int read_can_port;
int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0)
    {
        return (-1);
    }
    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);
    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0)
    {
        return (-1);
    }
    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        return (-1);
    }
    return 0;
}
int send_port(struct can_frame *frame)
{
    int retval;
   retval = write(soc, frame, sizeof(struct can_frame));
    if (retval != sizeof(struct can_frame))
    {
        return (-1);
    }
    else
    {
        return (0);
    }
}
/* this is just an example, run in a thread */
s_canframe frame_wr;

void read_port()
{
    struct can_frame frame_rd;

    int recvbytes = 0;
    read_can_port = 1;
    while(read_can_port)
    {
        struct timeval timeout = {1, 0};
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(soc, &readSet);
        if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
        {
            if (!read_can_port)
            {
                break;
            }
            if (FD_ISSET(soc, &readSet))
            {
                recvbytes = read(soc, &frame_rd, sizeof(struct can_frame));
                if(recvbytes)
                {
					printf("<< %03X (%d): ", frame_rd.can_id, frame_rd.can_dlc);
					int i = 0;
					for(i = 0; i < frame_rd.can_dlc; i++)
					{
						printf("%02X ", frame_rd.data[i]);
					}
					printf("\n");
					
					if(0 != obd_response(&frame_rd, &frame_wr))
					{
						printf(">> %03X (%d): ", frame_wr.can_id, frame_wr.can_dlc);
						int i = 0;
						for(i = 0; i < frame_wr.can_dlc; i++)
						{
							printf("%02X ", frame_wr.data[i]);
						}
						printf("\n");
						
						send_port(&frame_wr);
					}
					else
					{
						printf("No response\n");
					}
                }
            }
        }
    }
}
int close_port()
{
    close(soc);
    return 0;
}
int main(int argc, char **argv)
{
    open_port("can0");
    read_port();
	return 0;
}

