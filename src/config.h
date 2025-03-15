#ifndef CONFIG_H
#define CONFIG_H

#include <cjson/cJSON.h>
#include "spotifyAuth.h"

int file_exists(const char *filename);

creds *read_config();

void create_config(char refresh_code[132], char encoded_id[90], char client_id[33]);

#endif // CONFIG_H