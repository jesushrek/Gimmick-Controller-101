#include "Information.h"

#include <libusb-1.0/libusb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

/* 
 * [0x07, 0x09, active_byte, dpi_profile_byte, active_profiles_byte, 0x00, 0x00, 0x00]
 */

uint8_t prepare_color_mask(bool colors[7]);
uint8_t scale_color(int value);
uint8_t active_byte(int current_active_profile);
int snap_dpi(int dpi_count);
uint8_t dpi_profile_byte(int dpi_count, int profile_index);
uint8_t active_profile_byte(int states[6]);
void scroll_wheel_config(uint8_t* packet, bool volume);
uint8_t scale_colors(int value);
void build_dpi_payload(uint8_t *packet, int profile_index, int dpi_count, uint8_t active_mask);
void build_rgb_payload(uint8_t *packet, uint8_t cyclic_color_byte, char *scheme, int duration);
void build_color_payload(int profile_index, uint8_t *packet, uint8_t r, uint8_t g, uint8_t b, uint8_t active_profile_byte);
void sendPayload(libusb_device_handle *handle, uint8_t *packet);
