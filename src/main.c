#include "./Information.h"
#include "./Payload.h"
#include "./Config.h"
#include "./Types.h"
#include "./Driver.h"

#include <stdio.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>

const char* config_path = "/home/voyager-1/.config/gimmicks.csv";

int main() 
{ 
    Mouse mouse = {0};
    load_config(&mouse, config_path);

    libusb_context* ctx;
    int result = libusb_init_context(&ctx, NULL, 0);

    if (result)
        printf("Error code: %d", result);

    mouse_init(&mouse, ctx);
    mouse_apply(&mouse);
    mouse_close(&mouse, ctx);
    save_config(&mouse, config_path);

    return 0;
}
