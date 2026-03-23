#include "./Information.h"
#include "./Payload.h"
#include "./Config.h"
#include "./Types.h"

#include <stdio.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>


extern const char* config_path;

void mouse_init(Mouse* rat, libusb_context* ctx)
{ 
    rat->handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);

    if(!rat->handle)
    { 
        printf("Error couldn't handle the device.\n");
        exit(0);
    }

    if (libusb_kernel_driver_active(rat->handle, W_INDEX) == 1)
    { 
        printf("Detaching.\n");

        if (libusb_detach_kernel_driver(rat->handle, W_INDEX) == 0)
        {
            printf("Detached.\n");
        }
        else 
        { 
            printf("Couldn't detach.\n");
            exit(0);
        }
    }
    else 
    { 
        printf("kernal driver not active.\n");
    }

    int result = libusb_claim_interface(rat->handle, W_INDEX);
    if (result == 0)
    { 
        printf("Success: claimed!.\n");
    }
    else 
    { 
        printf("Error code: %d.\n", result);
        exit(0);
    }
}

void mouse_close(Mouse* rat, libusb_context* ctx)
{
    libusb_release_interface(rat->handle, W_INDEX);
    libusb_attach_kernel_driver(rat->handle, W_INDEX);
    libusb_close(rat->handle);
    libusb_exit(ctx);

    printf("Device freed!\n");
}

void mouse_apply(Mouse* rat)
{ 
    uint8_t packet[8] = {0};
    int states[6];
    for (int i = 0; i < 6; i++)
        states[i] = rat->profiles[i].is_active;

    uint8_t active_mask = active_profile_byte(states);

    for (int i = 0; i < 6; ++i)
    { 
        build_dpi_payload(packet, i + 1, rat->profiles[i].dpi_value, active_mask);
        sendPayload(rat->handle, packet);
    }

    rat->cyclic_color_mask = prepare_color_mask(rat->cycle_enabled_colors);

    build_rgb_payload(packet, rat->cyclic_color_mask, rat->rgb_scheme, rat->scheme_duration);
    sendPayload(rat->handle, packet);
    for (int i = 0; i < 6; ++i)
    { 
        build_color_payload(i+1, packet, 
                rat->profiles[i].red,
                rat->profiles[i].green,
                rat->profiles[i].blue,
                active_mask
                );
        sendPayload(rat->handle, packet);
    }

    rat->current_profile_index = rat->current_profile_index ? rat->current_profile_index : 1;

    build_dpi_payload(packet, rat->current_profile_index, rat->profiles[rat->current_profile_index - 1].dpi_value, active_mask);
    sendPayload(rat->handle, packet);

    scroll_wheel_config(packet, rat->volume_mode);
    sendPayload(rat->handle, packet);
}
