/*
 * Copyright (C) 2026 Sandesh Paudel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
