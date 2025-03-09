#include "spotifyAuth.h"
#include "base64.h"
#include <cjson/cJSON.h>
#include <cjson/cjson.h>
#include <curl/curl.h>
#include <memory.h>
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
// im just about so FUCKING GOATED AT THIS
// i wanna kill myself
struct server_data {
  char *auth_code;
};

struct token {
  char *access_code;
  char *refresh_code;
};

static int answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
  struct server_data *data = (struct server_data *)cls;
  const char *code;
  const char *response_str = "Success! Go back to the app!";
  struct MHD_Response *response;
  code = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "code");
  data->auth_code = (char *)malloc(strlen(code) + 1);
  strcpy(data->auth_code, code);
  response = MHD_create_response_from_buffer(
      strlen(response_str), (void *)response_str, MHD_RESPMEM_PERSISTENT);

  MHD_add_response_header(response, "Content-Type", "text/plain");

  int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

  MHD_destroy_response(response);

  return ret;
}

char *get_auth(char client_id[33]) {
  char url[197] = "https://accounts.spotify.com/authorize?client_id=";
  strcat(url, client_id);
  strcat(url, "&response_type=code&scope=user-read-currently-playing&redirect_uri=http://localhost:8888/callback&state=12345");
  struct MHD_Daemon *d;
  struct server_data data;
  d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, 8888, NULL, NULL,
                       &answer_to_connection, &data, NULL);
  printf("Please open the following url -> %s\n", url);
  getchar();
  MHD_stop_daemon(d);
  return data.auth_code;
}

void encode_id(char combined_id[89], char client_id[33], char secret_id[33]) {
  char combined[89];
  char *output;
  strcpy(combined, client_id);
  strcat(combined, ":");
  strcat(combined, secret_id);
  size_t output_size = 89;
  output = base64_encode(combined);
  printf("%s \n", combined);
  strcpy(combined_id, output);
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, char **str) {
  size_t new_len = (*str == NULL ? 0 : strlen(*str)) + size * nmemb;

  *str = realloc(*str, new_len + 1); // Reallocate memory for the new content
  if (*str == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }

  memcpy(*str + (new_len - size * nmemb), ptr, size * nmemb); // Append new data
  (*str)[new_len] = '\0'; // Null-terminate the string

  return size * nmemb;
}

struct token *get_access_n_refresh_token(char encoded_id[89], char *auth_code) {
  CURLcode ret;
  struct token *outcome = malloc(sizeof(struct token)); // Allocate memory for outcome
  if (outcome == NULL) {
    printf("Memory allocation failed for outcome\n");
    return NULL;
  }
  printf("Aliiiive\n");
  size_t total_size;
  CURL *hnd;
  cJSON *json;
  cJSON *access_token;
  cJSON *refresh_token;
  char *response = NULL;
  struct curl_slist *slist1;
  char data1[111] = "Authorization: Basic ";
  char data2[1000] = "grant_type=authorization_code&code=&redirect_uri=http://localhost:8888/callback";
  
  // Concatenate the authorization header and the post data
  strcat(data1, encoded_id);
  sprintf(data2, "grant_type=authorization_code&code=%s&redirect_uri=http://localhost:8888/callback", auth_code);
  printf("Hello? :%s\n", data1);
  printf("Hello2? :%s\n", data2);
  
  // Calculate total size of the data to be sent (after concatenating both parts)
  total_size = strlen(data1) + strlen(data2);
  
  // Prepare HTTP headers
  slist1 = NULL;
  slist1 = curl_slist_append(slist1, data1);
  slist1 = curl_slist_append(slist1, "Content-Type: application/x-www-form-urlencoded");

  // Initialize cURL and set options
  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, "https://accounts.spotify.com/api/token");
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, data2);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)total_size);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.9.1");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);

  // Perform the request
  ret = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;
  
  printf("%s \n", response);

  // Parse the response to get access token and refresh token
  json = cJSON_Parse(response);

  access_token = cJSON_GetObjectItemCaseSensitive(json, "access_token");
  if (access_token == NULL) {
    printf("Couldn't get access_token\n");
    free(outcome);
    return NULL;
  }
  printf("Congrats, we got it!!!\n");
  printf("%s \n", access_token->valuestring);

  // Copy the access token properly into the struct
  outcome->access_code = strdup(access_token->valuestring); // strdup allocates memory and copies the string
  if (outcome->access_code == NULL) {
    printf("Failed to copy access token\n");
    free(outcome);
    return NULL;
  }

  refresh_token = cJSON_GetObjectItemCaseSensitive(json, "refresh_token");

  if (refresh_token == NULL) {
    printf("Couldn't get refresh_token\n");
    free(outcome->access_code);  // Clean up previously allocated memory
    free(outcome);
    return NULL;
  }

  // Copy the refresh token properly into the struct
  outcome->refresh_code = strdup(refresh_token->valuestring); // strdup allocates memory and copies the string
  if (outcome->refresh_code == NULL) {
    printf("Failed to copy refresh token\n");
    free(outcome->access_code);
    free(outcome);
    return NULL;
  }

  free(response);  // Free the response string
  cJSON_Delete(json); // Free the parsed JSON object
  return outcome;
}


creds *get_spotify_credentials(char client_id[33], char secret_id[33]) {
  creds *credentials;
  struct token *codes;
  char combined_id[89];
  char *auth;
  credentials = malloc(sizeof(creds));
  printf("Nice \n");
  auth = get_auth(client_id);
  printf("hello...?\n");
  encode_id(combined_id, client_id, secret_id);
  printf("checkpoint 1\n");
  strcpy(credentials->encoded_id, combined_id);
  printf("checkpoint 2\n");
  printf("Combined id? %s\n", combined_id);
  printf("IM STILL STANDING %s \n", auth);
  codes = get_access_n_refresh_token(combined_id, auth);
  strcpy(credentials->refresh_token, codes->refresh_code);
  free(codes);
  return credentials;
}

void refresh_token(char access_token[275], char refresh_token[132],
                   char encoded_id[90], char client_id[33]) {
  CURLcode ret;
  CURL *hnd;
  cJSON *json;
  cJSON *json_access;
  char *response = NULL;
  struct curl_slist *slist1;
  char data1[110] = "Authorization: Basic ";
  char data2[217] = "grant_type=refresh_token&refresh_token=";
  slist1 = NULL;
  strcat(data1, encoded_id);
  printf("%s %s %s\n", refresh_token, encoded_id, client_id);
  sprintf(data2, "grant_type=refresh_token&refresh_token=%s&client_id=%s", refresh_token, client_id);
  slist1 = curl_slist_append(slist1, data1);
  slist1 = curl_slist_append(slist1, "User-Agent: curl/8.9.1");
  printf("%s\n", data1);
  printf("%s\n", data2);
  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, "https://accounts.spotify.com/api/token");
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, data2);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)213);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.9.1");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);
  ret = curl_easy_perform(hnd);
  printf("%s \n", response);
  json = cJSON_Parse(response);
  json_access = cJSON_GetObjectItemCaseSensitive(json, "access_token");
  strcat(access_token, json_access->valuestring);
  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;
}