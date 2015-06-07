#include "network.h"

#ifdef USE_IWINFO

#include "backend_iwinfo.h"

#else

#include "backend_none.h"

#endif

int debug = 0;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: mlsc <device> [-d]\n");
        return 1;
    }
    char *device = argv[1];
    if (argc >= 3 && strcmp("-d", argv[2]) == 0) {
        debug = 1;
    }
    char *req = mlsc_backend_create_request(device, debug);
    if (!req)
        return 21;
    char *resp = mlsc_network_request(req, debug);
    free(req);
    if (!resp)
        return 22;

    char *token, *string, *tofree;
    tofree = string = strdup(resp);
    int split = 0;
    while ((token = strsep(&string, "\n")) != 0) {
        if (split) {
            printf("%s\n", token);
        }
        if (*token == '\r') {
            split = 1;
        }
    }
    free(tofree);
    free(resp);
    return 0;
}