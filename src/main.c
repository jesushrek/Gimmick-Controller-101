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
#include <string.h>
#include <libusb-1.0/libusb.h>

int main(int argc, char* argv[]) 
{ 
    char* config_path = NULL;

    for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], "-path") == 0 && (i + 1) < argc)
            config_path = strdup(argv[++i]);

    if (config_path == NULL)
        config_path = get_config_path();

    Mouse mouse = {0};
    load_config(&mouse, config_path);
    libusb_context* ctx = NULL;

    if (libusb_init_context(&ctx, NULL, 0)) {
        printf("Failed to init libusb\n");
        return 1;
    }

    bool should_save = false;

    parse_arguments(argc, argv, &mouse, &should_save);
    mouse_init(&mouse, ctx);
    mouse_apply(&mouse);
    mouse_close(&mouse, ctx);

    if (should_save) save_config(&mouse, config_path);
    return 0;
}
