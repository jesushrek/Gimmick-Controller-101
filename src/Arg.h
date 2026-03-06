#ifndef arg_h
#define arg_h

#include "./Information.h"
#include "./Payload.h"
#include "./Config.h"
#include "./Types.h"
#include "./Driver.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>

static void hex_to_rgb(const char* hex, uint8_t* r, uint8_t* g, uint8_t* b)
{ 
    unsigned int tr = 0;
    unsigned int tg = 0;
    unsigned int tb = 0;

    if (sscanf(hex, "%02x%02x%02x", &tr, &tg, &tb) == 3)
    { 
        *r = (uint8_t)tr;
        *g = (uint8_t)tg;
        *b = (uint8_t)tb;
    }
}

bool parse_arguments(int argc, char* argv[], Mouse *rat)
{ 
    if (argc <= 1) return false;
    for (int i = 1; i < argc; ++i)
    { 
        if (strcmp(argv[i], "-p") == 0 && (i + 1) < argc)
        { 
            int input_profile = atoi(argv[++i]);
            rat->current_profile_index = ((input_profile - 1) % 6 + 6) % 6 + 1;
        }
        else if (strcmp(argv[i], "-dpi") == 0 && (i + 1) < argc)
        { 
            int idx = rat->current_profile_index - 1;
            rat->profiles[idx].dpi_value = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-color") == 0 && (i + 1) < argc)
        { 
            int idx = rat->current_profile_index - 1;
            hex_to_rgb(
                    argv[++i],
                    &rat->profiles[idx].red,
                    &rat->profiles[idx].green,
                    &rat->profiles[idx].blue
                    );
        }
        else if (strcmp(argv[i], "-sync") == 0)
        { 
            return true;
        }
    }

    return true;
}

#endif
