#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef struct
{
    char *current_song;
    char *current_artist;
    char *current_album;
    char *cover_url;
    int is_playing;
    int success; // 1 is fail, 0 is good
}song;

song get_current_song(char *access_token);

#endif //SPOTIFYAPI_H

