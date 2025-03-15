#define _CRT_SECURE_NO_WARNINGS
#include "SpotifyAPI.h"
#include <cjson/cJSON.h>

size_t writefunced(void *ptr, size_t size, size_t nmemb, char **str) {
  size_t new_len = (*str == NULL ? 0 : strlen(*str)) + size * nmemb;

  *str = realloc(*str, new_len + 1);
  if (*str == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }

  memcpy(*str + (new_len - size * nmemb), ptr, size * nmemb);
  (*str)[new_len] = '\0';

  return size * nmemb;
}

song get_current_song(char *access_code) {
    song music_info;
    CURLcode ret;
    CURL *hnd;
    cJSON *json;
    cJSON *current_song;
    cJSON *current_image;
    cJSON *current_artist;
    cJSON *current_album;
    cJSON *item;
    cJSON *album;
    cJSON *images;
    cJSON *artists;
    cJSON *first_image;
    cJSON *is_playing;
    char *response = NULL;
    struct curl_slist *slist1;
    char data[298] = "Authorization: Bearer ";
    strcat(data, access_code);
    slist1 = NULL;
    slist1 = curl_slist_append(slist1, data);
  
    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL,
                     "https://api.spotify.com/v1/me/player/currently-playing");
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.9.1");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunced);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);
  
    ret = curl_easy_perform(hnd);
    if (response == NULL) {
      music_info.success = 1;
      return music_info;
    }
  
    json = cJSON_Parse(response);
    if (!json) {
      music_info.success = 1;
      return music_info;
    }
  
    // Accessing the "is_playing" field directly from the JSON root
    is_playing = cJSON_GetObjectItemCaseSensitive(json, "is_playing");
    if (is_playing) {
      if (cJSON_IsTrue(is_playing) == 1) {
        music_info.is_playing = 1;
      } else {
        music_info.is_playing = 0;
      }
    }
  
    item = cJSON_GetObjectItemCaseSensitive(json, "item");
  
    current_song = cJSON_GetObjectItemCaseSensitive(item, "name");
    album = cJSON_GetObjectItemCaseSensitive(item, "album");
    images = cJSON_GetObjectItemCaseSensitive(album, "images");
    first_image = cJSON_GetArrayItem(images, 0);
    current_image = cJSON_GetObjectItemCaseSensitive(first_image, "url");
    current_album = cJSON_GetObjectItemCaseSensitive(album, "name");
    artists = cJSON_GetObjectItemCaseSensitive(item, "artists");
    cJSON *first_artist = cJSON_GetArrayItem(artists, 0);
    current_artist = cJSON_GetObjectItemCaseSensitive(first_artist, "name");
  
    // Directly assign the memory
    music_info.current_song = strdup(current_song->valuestring);
    music_info.current_album = strdup(current_album->valuestring);
    music_info.current_artist = strdup(current_artist->valuestring);
    music_info.cover_url = strdup(current_image->valuestring);
    
    // Set success flag based on presence of necessary data
    if (current_song && current_album && current_artist && current_image) {
      music_info.success = 0;
    } else {
      music_info.success = 1;
    }
  
    // Freeing up resources
    free(response);  // Free the response buffer
    curl_easy_cleanup(hnd);  // Clean up the CURL handle
    curl_slist_free_all(slist1);  // Free the list of headers
    
    // Delete cJSON objects
    cJSON_Delete(json);  // This will free the entire JSON object, including nested objects
  
    return music_info;
  }
  