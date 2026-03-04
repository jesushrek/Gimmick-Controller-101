#ifndef CONFIG_H
#define CONFIG_H
#include "./Types.h"

void load_config(Mouse* rat, const char* config_path);
void save_config(Mouse* rat, const char* config_path);
char* get_config_path();

#endif
