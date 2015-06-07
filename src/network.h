#ifndef MLSC_NETWORK_H
#define MLSC_NETWORK_H

#include <stdlib.h>
#include "global.h"

#define SERVER_PORT 443
#define SERVER_NAME "location.services.mozilla.com"
#define POST_REQUEST "POST /v1/geolocate?key=test HTTP/1.1\r\nUser-Agent: mlsc/1.0\r\nHost: location.services.mozilla.com\r\nAccept: */*\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: close\r\n\r\n%s"

#if !defined(POLARSSL_CONFIG_FILE)

#include "polarssl/config.h"

#else
#include POLARSSL_CONFIG_FILE
#endif

#include "polarssl/net.h"
#include "polarssl/debug.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"
#include "polarssl/error.h"
#include "polarssl/certs.h"

char *mlsc_network_request(char *request, int debug_level);

#endif //MLSC_NETWORK_H
