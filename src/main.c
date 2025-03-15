#define _CRT_SECURE_NO_WARNINGS

#include "SpotifyAPI.h"
#include "config.h"
#include "spotifyAuth.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// to make the program work, input your client id as the first program argument, and the secret id as the second argument
int main(int argc, char *argv[]) {
  creds *credentials;
  song music_info;
  if (file_exists("config.json") == 1) {
    printf("config not found, creating...\n");
    credentials = get_spotify_credentials(argv[1], argv[2]);
    create_config(credentials->refresh_token, credentials->encoded_id,
                  argv[1]);
    free(credentials);
  } else {
    char *new_access_token;
    char *current_song;
    printf("config found, hooray!\n");
    credentials = read_config();
    new_access_token = refresh_token(new_access_token, credentials->refresh_token,
                  credentials->encoded_id, credentials->client_id);
    printf("Heyeee%s\n", new_access_token);
    music_info = get_current_song(new_access_token);
    current_song = music_info.current_song;

    current_song = strdup("");
      
    for (;;) {
      music_info = get_current_song(new_access_token);

      if (music_info.success != 0) {
        break;
      }

      if (strcmp(music_info.current_song, current_song) == 0) {
        printf("Same song being played->%s\n", music_info.current_song);
      } else {
        printf("New song is being played->%s\n", music_info.current_song);

        free(current_song);
        current_song = strdup(music_info.current_song);
      }

      free(music_info.current_song);
      free(music_info.current_album);
      free(music_info.current_artist);
      free(music_info.cover_url);

      Sleep(10);
    }
    free(current_song);
    free(new_access_token);
  }
}