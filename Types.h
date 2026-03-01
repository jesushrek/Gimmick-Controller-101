#ifndef TYPE_H
#define TYPE_H

#include <libusb-1.0/libusb.h>
#include <stdbool.h>

typedef struct 
{ 
    int dpi_value;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    bool is_active;

} Profile_config;

typedef struct 
{ 
    libusb_device_handle* handle;

    int current_profile_index;
    char rgb_scheme[16];
    int scheme_duration;
    uint8_t cyclic_color_mask;
    bool cycle_enabled_colors[7];

    Profile_config profiles[6];
    bool volume_mode;

} Mouse;

#endif
