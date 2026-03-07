#include "./Information.h"
#include "./Payload.h"
#include "./Config.h"
#include "./Types.h"
#include "./Driver.h"
#include "./Arg.h"

#include <stdio.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>


int main(int argc, char* argv[]) 
{ 
    char* config_path = get_config_path();
    Mouse mouse = {0};
    load_config(&mouse, config_path);

    libusb_context* ctx;
    int result = libusb_init_context(&ctx, NULL, 0);

    if (result)
        printf("Error code: %d", result);

    mouse_init(&mouse, ctx);

    if (!parse_arguments(argc, argv, &mouse))
    {
        printf("Usage: %s [-p 1-6] [-dpi value] [-color RRGGBB] [-sync]\n", argv[0]);
        return -1;
    }

    mouse_apply(&mouse);
    mouse_close(&mouse, ctx);
    //save_config(&mouse, config_path);
    free(config_path);

    return 0;
}
