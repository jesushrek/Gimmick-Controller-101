#include "Information.h"

#include <libusb-1.0/libusb.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

/* 
 * [0x07, 0x09, active_byte, dpi_profile_byte, active_profiles_byte, 0x00, 0x00, 0x00]
 */

typedef enum {
    M_YELLOW = 0x01, // Bit 0
    M_BLUE   = 0x02, // Bit 1
    M_VIOLET = 0x04, // Bit 2
    M_GREEN  = 0x08, // Bit 3
    M_RED    = 0x10, // Bit 4
    M_CYAN   = 0x20, // Bit 5
    M_WHITE  = 0x40  // Bit 6
} MouseColor;

uint8_t prepare_color_mask(bool colors[7])
{ 
    uint8_t mask = 0x0;
    if (colors[0]) mask |= M_YELLOW;
    if (colors[1]) mask |= M_BLUE;
    if (colors[2]) mask |= M_VIOLET;
    if (colors[3]) mask |= M_GREEN;
    if (colors[4]) mask |= M_RED;
    if (colors[5]) mask |= M_CYAN;
    if (colors[6]) mask |= M_WHITE;

    return mask;
}

uint8_t active_profile_byte(int states[6])
{ 
    return  ( 
            states[0] << 0 | states[1] << 1 | states[2] << 2 | states[3] << 3 | states[4] << 4 | states[5] << 5
            );
}

int snap_dpi(int dpi_count)
{     
    int g_DPI_ARR[] = { 200, 400, 600, 800, 1000, 1200, 1600, 2000, 2400, 3200, 4000, 4800, };

    size_t total_dpi = sizeof(g_DPI_ARR) / sizeof(*g_DPI_ARR);

    int distance = 100000;
    int best_index = 0;

    for (size_t i = 0; i < total_dpi; ++i)
    { 
        int new_distance = abs(dpi_count - g_DPI_ARR[i]);

        if (distance > new_distance)
        {
            distance = new_distance;
            best_index = i;
        }
    }

    return g_DPI_ARR[best_index];
}

uint8_t dpi_profile_byte(int dpi_count, int profile_index)
{ 
    int snapped_dpi = snap_dpi(dpi_count);

    uint8_t instruction = 0x0;

    if (snapped_dpi <= 1200)
        instruction = (uint8_t)(snapped_dpi / 200);
    else if(snapped_dpi == 1600)
        instruction = 0x7;
    else if(snapped_dpi == 2000)
        instruction = 0x9;
    else if(snapped_dpi == 2400)
        instruction = 0xB;
    else if(snapped_dpi == 3200)
        instruction = 0xD;
    else if(snapped_dpi == 4000)
        instruction = 0xE;
    else if(snapped_dpi == 4800)
        instruction = 0xF;

    return (instruction << 4) | (profile_index + 7);

}

uint8_t scroll_wheel_config(bool volume)
{ 
    return volume;
}

uint8_t scale_colors(int value)
{ 
    return (uint8_t)((255 - value) / 16);
}

uint8_t active_byte(int profile_index)
{ 
    return (uint8_t)(0x40 - 1 + profile_index);
}

void build_dpi_payload(uint8_t *packet, int profile_index, int dpi_count, uint8_t active_mask)
{ 
    memset(packet, 0, INSTRUCTION_LENGTH);
    packet[0] = 0x07;
    packet[1] = 0x09;
    packet[2] = active_byte(profile_index);
    packet[3] = dpi_profile_byte(dpi_count, profile_index);
    packet[4] = active_mask;
}

void build_rgb_payload(uint8_t *packet, uint8_t cyclic_color_byte, char *scheme, int duration)
{ 
    memset(packet, 0, INSTRUCTION_LENGTH);
    packet[0] = 0x07;
    packet[1] = 0x13;
    packet[2] = cyclic_color_byte;

    if (strcmp(scheme, "Fixed") == 0)
        packet[3] = 0x86 - duration;
    if (strcmp(scheme, "Cyclic") == 0)
        packet[3] = 0x96 - duration;
    if (strcmp(scheme, "Static") == 0)
        packet[3] = 0x86;
    if (strcmp(scheme, "Off") == 0)
        packet[3] = 0x87;
}

void build_color_payload(int profile_index, uint8_t *packet, uint8_t r, uint8_t g, uint8_t b, uint8_t active_profile_byte)
{ 
    memset(packet, 0, INSTRUCTION_LENGTH);
    uint8_t internal_red   = scale_colors(r);
    uint8_t internal_green = scale_colors(g);
    uint8_t internal_blue  = scale_colors(b);

    uint8_t internal_profile = (profile_index - 1) * 2;

    packet[0] = 0x07;
    packet[1] = 0x14;
    packet[2] = (internal_profile << 4) | (internal_green & 0xF);
    packet[3] = (internal_red << 4) | (internal_blue & 0xF);
    packet[4] = active_profile_byte;
}

void sendPayload(libusb_device_handle *handle, uint8_t *packet)
{
    libusb_control_transfer(handle, BM_REQUEST_TYPE, B_REQUEST, W_VALUE, W_INDEX, packet, 8, 1000);
}

