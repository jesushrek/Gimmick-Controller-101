#include "./Types.h"
#include "./Payload.h"

#include <libusb-1.0/libusb.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

char* get_config_path()
{ 
    struct passwd *p;
    uid_t uid;

    if ((p = getpwuid(uid = getuid())) == NULL)
    {
        perror("getpwuid() error");
    }

    char* buffer = malloc(sizeof(char) * 255);
    sprintf(buffer, "/home/%s/.config/gimmicks.csv", p->pw_name);

    return buffer;
}

void load_config(Mouse* rat, const char* config_path)
{
    FILE* file = fopen(config_path, "r");
    if (!file) 
    { 
        printf("The file doesn't exist?\n");
        exit(0);
    }

    char line[100];
    while (fgets(line, sizeof(line), file))
    {
        if ((line[0] == '#') || (line[0] == '\n')) continue;

        if (strncmp(line, "Scroll", 6) == 0)
        { 
            int temp = 0;
            sscanf(line, "Scroll, %d", &temp);
            rat->volume_mode = temp;
        }

        if (strncmp(line, "GLOBAL", 6) == 0)
        { 
            int temp[7];
            sscanf(line, "GLOBAL,%[^,],%d,%d,%d,%d,%d,%d,%d,%d", 
                    rat->rgb_scheme,
                    &rat->scheme_duration,
                    &temp[0],
                    &temp[1],
                    &temp[2],
                    &temp[3],
                    &temp[4],
                    &temp[5],
                    &temp[6]
                  );

            for (int i = 0; i < 7; ++i)
            {
                rat->cycle_enabled_colors[i] = (bool)temp[i];
            }

            rat->cyclic_color_mask = prepare_color_mask(rat->cycle_enabled_colors);
        }
        else
        { 
            int idx, dpi, r, g, b, active;
            int parsed = sscanf(line, "%d,%d,%d,%d,%d,%d", &idx, &dpi, &r, &g, &b, &active);
            if (parsed == 6) 
            { 
                if (idx >= 1 && idx <= 6)
                {
                    rat->profiles[idx - 1].dpi_value = dpi;
                    rat->profiles[idx - 1].red = r;
                    rat->profiles[idx - 1].green = g;
                    rat->profiles[idx - 1].blue = b;
                    rat->profiles[idx - 1].is_active = active;
                }
            }
        }
    }

    fclose(file);
}

void save_config(Mouse* rat, const char* config_path)
{ 
    FILE* file = fopen(config_path, "w");
    if (!file)
    { 
        printf("The file doesn't exist?\n");
        exit(0);
    }

    fprintf(file, "#Profile, DPI, RED, Green, Blue, Active\n");
    for (int i = 0; i < 6; ++i)
    { 
        fprintf(file, "%d,%d,%d,%d,%d,%d\n", i + 1, 

                rat->profiles[i].dpi_value, 
                rat->profiles[i].red, 
                rat->profiles[i].green, 
                rat->profiles[i].blue, 
                rat->profiles[i].is_active? 1 : 0
               );
    }

    fprintf(file, "#Scheme,Duration, (Y, B, V, G, R, C, W)\n");
    fprintf(file, "GLOBAL,%s,%d,%d,%d,%d,%d,%d,%d,%d\n", 
            rat->rgb_scheme,
            rat->scheme_duration,
            rat->cycle_enabled_colors[0]? 1 : 0,
            rat->cycle_enabled_colors[1]? 1 : 0,
            rat->cycle_enabled_colors[2]? 1 : 0,
            rat->cycle_enabled_colors[3]? 1 : 0,
            rat->cycle_enabled_colors[4]? 1 : 0,
            rat->cycle_enabled_colors[5]? 1 : 0,
            rat->cycle_enabled_colors[6]? 1 : 0
           ); 
    fprintf(file, "# 1 for volume, 0 for scroll\n");
    fprintf(file, "Scroll, %d\n", rat->volume_mode);

    fclose(file);
}
