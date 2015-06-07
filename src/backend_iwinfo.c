#include "backend_iwinfo.h"

char *mlsc_backend_create_request(char *device, int debug_level) {
    if (debug_level) fprintf(stderr, "  . Using libiwinfo.\n");
    char *req = malloc(BUFFER_SIZE);
    const struct iwinfo_ops *iw = iwinfo_backend(device);
    if (!iw) {
        fprintf(stderr, "  ! Failed to initialize iwinfo for device %s\n", device);
        free(req);
        return NULL;
    }
    int len = 0;
    char buf[IWINFO_BUFSIZE];
    if (!iw->scanlist(device, buf, &len)) {
        strcat(req, "{\"wifiAccessPoints\": [");
        if (debug_level)
            fprintf(stderr, "  . Get %d bytes result, thats %d entries\n", len,
                    (int) (len / sizeof(struct iwinfo_scanlist_entry)));
        int x = 0;
        for (int i = 0; i < len; i += sizeof(struct iwinfo_scanlist_entry)) {
            struct iwinfo_scanlist_entry *entry = (struct iwinfo_scanlist_entry *) &buf[i];
            if (debug_level) fprintf(stderr, "  . Parsing bytes at %d for entry %d \n", i, x);
            if (x != 0) {
                strcat(req, ",");
            }
            x++;
            snprintf((req + strlen(req)), BUFFER_SIZE - strlen(req) - 3,
                     "{\"macAddress\": \"%x:%x:%x:%x:%x:%x\", \"signalStrength\": %d}",
                     entry->mac[0], entry->mac[1], entry->mac[2], entry->mac[3], entry->mac[4], entry->mac[5],
                     ((int) entry->signal) - 256);
        }
        strcat(req, "]}");
    } else {
        fprintf(stderr, "  ! Failed to scan device %s\n", device);
        free(req);
        req = NULL;
    }
    iwinfo_finish();
    return req;
}