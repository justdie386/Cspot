#include "config.h"
#include "spotifyAuth.h"
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

int file_exists(const char *filename) {
  struct stat buffer;
  return stat(filename, &buffer) == 0 ? 0 : 1;
}

creds *read_config() {
  cJSON *json;
  cJSON *client_id;
  cJSON *refresh_token;
  cJSON *encoded_id;

  char *file_content;
  char *json_printed;
  creds *credentials;
  FILE *file;
  size_t file_size;

  // Open the file
  file = fopen("config.json", "rb");
  if (file == NULL) {
    printf("File not found!\n");
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // Allocate memory for file content
  file_content = (char *)malloc(file_size + 1);
  if (file_content == NULL) {
    printf("Memory allocation failed!\n");
    return NULL;
  }

  fread(file_content, 1, file_size, file);
  file_content[file_size] = '\0';
  printf("File size: %zu\n", file_size);

  json = cJSON_Parse(file_content);
  if (json == NULL) {
    printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
    free(file_content);
    return NULL;
  }
  json_printed = cJSON_Print(json);
  printf("File content: %s\n", json_printed);
  credentials = malloc(sizeof(creds));
  refresh_token = cJSON_GetObjectItemCaseSensitive(json, "refresh_token");
  client_id = cJSON_GetObjectItemCaseSensitive(json, "client_id");
  encoded_id = cJSON_GetObjectItemCaseSensitive(json, "encoded_id");

  strcpy(credentials->refresh_token, refresh_token->valuestring);
  strcpy(credentials->client_id, client_id->valuestring);
  strcpy(credentials->encoded_id, encoded_id->valuestring);

  free(file_content);
  free(json_printed);

  return credentials;
}

void create_config(char refresh_code[132], char encoded_id[90],
                     char client_id[33]) {
  cJSON *json;
  char *json_str;
  FILE *file;
  file = fopen("config.json", "w");
  printf("%s %s \n", refresh_code, encoded_id);
  json = cJSON_CreateObject();

  cJSON_AddStringToObject(json, "refresh_token", refresh_code);
  cJSON_AddStringToObject(json, "encoded_id", encoded_id);
  cJSON_AddStringToObject(json, "client_id", client_id);
  json_str = cJSON_Print(json);
  fputs(json_str, file);
  fclose(file);
  cJSON_free(json_str);
  cJSON_Delete(json);
  printf("Config created!\n");
}
