#ifndef MLSC_BACKEND_NONE_H
#define MLSC_BACKEND_NONE_H

char *mlsc_backend_create_request(char *device, int debug_level) {
    if (debug_level) fprintf(stderr, "  ! No wifi backend configured, request based on IP.\n");
    char* res = malloc(3);
    strcpy(res, "{}");
    return res;
}

#endif //MLSC_BACKEND_NONE_H
