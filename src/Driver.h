#ifndef DRIVER_H
#define DRIVER_H

#include "./Types.h"
#include <libusb-1.0/libusb.h>


void mouse_init(Mouse* rat, libusb_context* ctx);
void mouse_close(Mouse* rat, libusb_context* ctx);
void mouse_apply(Mouse* rat);

#endif
