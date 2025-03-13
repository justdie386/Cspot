#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS
#include "config.h"
#include "spotifyAuth.h"
#include "SpotifyAPI.h"
#include <crtdbg.h>
#include <stdio.h>
#include <windows.h>
#define CLIENTID
#define SECRETID

int check_song(song music_info)
{
  if (music_info.success == 1)
  {
    printf("Failed to grab the song... exiting!\n");
    return 1;
  }
  printf("We got the song!!!->%s\n", music_info.current_song);
  printf("The image of the cover is at the url->%s\n", music_info.cover_url);
  if (music_info.is_playing == 0)
  {
    printf("Music is currently paused!\n");
  }
  else {
    printf("Music is playing!!!\n");
  }
  return 0;
}
int main(int argc, char *argv[]) {
  _CrtCheckMemory();
  creds *credentials;
  song music_info;
  if (file_exists("config.json") == 1) {
    printf("config not found, creating...\n");
    credentials = get_spotify_credentials(CLIENTID, SECRETID);
    create_config(credentials->refresh_token, credentials->encoded_id, CLIENTID);
    free(credentials);
  } else {
    char new_access_token[275];
    char *current_song;
    printf("config found, hooray!\n");
    credentials = read_config();
    refresh_token(new_access_token, credentials->refresh_token, credentials->encoded_id, credentials->client_id);
    printf("%s\n", new_access_token);
    music_info = get_current_song(new_access_token);
    current_song = music_info.current_song;
    while (TRUE)
    {
        music_info = get_current_song(new_access_token);
    
        if (music_info.success != 0) {
            break;
        }
    
        if (strcmp(music_info.current_song, current_song) != 0)
        {
            printf("New song is being played! ->%s\n", music_info.current_song);
            current_song = music_info.current_song;
        }
        else {
            printf("Same music still playing ->%s\n", current_song);
        }
    
        free(music_info.current_song);
        free(music_info.current_album);
        free(music_info.current_artist);
        free(music_info.cover_url);
        
        Sleep(1000);
    }
  }
}
