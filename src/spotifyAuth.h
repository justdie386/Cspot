#ifndef SPOTIFYAUTH_H
#define SPOTIFYAUTH_H

typedef struct
{
    char refresh_token[132];
    char encoded_id[90];
    char client_id[33];
} creds;

creds *get_spotify_credentials(char client_id[33], char secret_id[33]);

char *refresh_token(char *access_token, char refresh_token[132], char encoded_id[90],char client_id[33]);

#endif //SPOTIFYAUTH_H