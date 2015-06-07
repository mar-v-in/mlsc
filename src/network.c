/*
 *  Based on the SSL client demonstration program of polarssl.
 *
 *  Copyright (C) 2006-2013, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "network.h"

#if !defined(POLARSSL_BIGNUM_C) || !defined(POLARSSL_ENTROPY_C) || \
    !defined(POLARSSL_SSL_TLS_C) || !defined(POLARSSL_SSL_CLI_C) || \
    !defined(POLARSSL_NET_C) || !defined(POLARSSL_RSA_C) || \
    !defined(POLARSSL_CTR_DRBG_C) || !defined(POLARSSL_X509_CRT_PARSE_C)

char *mlsc_network_request(char *request) {
    fprintf(stderr, "POLARSSL_BIGNUM_C and/or POLARSSL_ENTROPY_C and/or "
            "POLARSSL_SSL_TLS_C and/or POLARSSL_SSL_CLI_C and/or "
            "POLARSSL_NET_C and/or POLARSSL_RSA_C and/or "
            "POLARSSL_CTR_DRBG_C and/or POLARSSL_X509_CRT_PARSE_C "
            "not defined.\n");
    return NULL;
}

#else

char *mlsc_network_request(char *request, int debug_level) {
    int ret, len, server_fd = -1;
    char tmpbuf[BUFFER_SIZE];
    char *buf = malloc(BUFFER_SIZE);
    const char *pers = "ssl_client1";

    entropy_context entropy;
    ctr_drbg_context ctr_drbg;
    ssl_context ssl;
    x509_crt cacert;

#if defined(POLARSSL_DEBUG_C)
    if (debug_level) debug_set_threshold(1);
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
    memset(&ssl, 0, sizeof(ssl_context));
    x509_crt_init(&cacert);

    if (debug_level) fprintf(stderr, "\n  . Seeding the random number generator...");


    entropy_init(&entropy);
    if ((ret = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                             (const unsigned char *) pers,
                             strlen(pers))) != 0) {
        if (debug_level) fprintf(stderr, " failed\n  ! ctr_drbg_init returned %d\n", ret);
        goto exit;
    }

    if (debug_level) fprintf(stderr, " ok\n");

    /*
     * 0. Initialize certificates
     */
    if (debug_level) fprintf(stderr, "  . Loading the CA root certificate ...");
    fflush(stdout);

#if defined(POLARSSL_CERTS_C)
    ret = x509_crt_parse(&cacert, (const unsigned char *) test_ca_list,
                         strlen(test_ca_list));
#else
    ret = 1;
    if (debug_level) fprintf(stderr, "POLARSSL_CERTS_C not defined.");
#endif

    if (ret < 0) {
        if (debug_level) fprintf(stderr, " failed\n  !  x509_crt_parse returned -0x%x\n\n", -ret);
        goto exit;
    }

    if (debug_level) fprintf(stderr, " ok (%d skipped)\n", ret);

    /*
     * 1. Start the connection
     */
    if (debug_level)
        fprintf(stderr, "  . Connecting to tcp/%s/%4d...", SERVER_NAME,
                SERVER_PORT);

    if ((ret = net_connect(&server_fd, SERVER_NAME,
                           SERVER_PORT)) != 0) {
        if (debug_level) fprintf(stderr, " failed\n  ! net_connect returned %d\n\n", ret);
        goto exit;
    }

    if (debug_level) fprintf(stderr, " ok\n");

    /*
     * 2. Setup stuff
     */
    if (debug_level) fprintf(stderr, "  . Setting up the SSL/TLS structure...");

    if ((ret = ssl_init(&ssl)) != 0) {
        if (debug_level) fprintf(stderr, " failed\n  ! ssl_init returned %d\n\n", ret);
        goto exit;
    }

    if (debug_level) fprintf(stderr, " ok\n");

    ssl_set_endpoint(&ssl, SSL_IS_CLIENT);
    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    ssl_set_authmode(&ssl, SSL_VERIFY_OPTIONAL);
    ssl_set_ca_chain(&ssl, &cacert, NULL, SERVER_NAME);

    ssl_set_rng(&ssl, ctr_drbg_random, &ctr_drbg);
    ssl_set_bio(&ssl, net_recv, &server_fd,
                net_send, &server_fd);

    /*
     * 4. Handshake
     */
    if (debug_level) fprintf(stderr, "  . Performing the SSL/TLS handshake...");

    while ((ret = ssl_handshake(&ssl)) != 0) {
        if (ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
            fprintf(stderr, " failed\n  ! ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }

    if (debug_level) fprintf(stderr, " ok\n");

    /*
     * 5. Verify the server certificate
     */
    if (debug_level) fprintf(stderr, "  . Verifying peer X.509 certificate...");

    /* In real life, we may want to bail out when ret != 0 */
    if ((ret = ssl_get_verify_result(&ssl)) != 0) {
        if (debug_level) fprintf(stderr, " failed\n");

        if ((ret & BADCERT_EXPIRED) != 0 && debug_level) fprintf(stderr, "  ! server certificate has expired\n");

        if ((ret & BADCERT_REVOKED) != 0 && debug_level) fprintf(stderr, "  ! server certificate has been revoked\n");

        if ((ret & BADCERT_CN_MISMATCH) != 0 && debug_level)
            fprintf(stderr, "  ! CN mismatch (expected CN=%s)\n", SERVER_NAME);

        if ((ret & BADCERT_NOT_TRUSTED) != 0 && debug_level)
            fprintf(stderr, "  ! self-signed or not signed by a trusted CA\n");

        if (debug_level) fprintf(stderr, "\n");
    }
    else if (debug_level) fprintf(stderr, " ok\n");

    /*
     * 3. Write the GET request
     */
    if (debug_level) fprintf(stderr, "  > Write to server:");

    len = sprintf((char *) tmpbuf, POST_REQUEST, strlen(request), request);

    while ((ret = ssl_write(&ssl, tmpbuf, len)) <= 0) {
        if (ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
            if (debug_level) fprintf(stderr, " failed\n  ! ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }

    len = ret;
    if (debug_level) fprintf(stderr, " %d bytes written\n\n%s", len, (char *) tmpbuf);

    /*
     * 7. Read the HTTP response
     */
    if (debug_level) fprintf(stderr, "  < Read from server:");

    do {
        len = BUFFER_SIZE - 1;
        memset(tmpbuf, 0, BUFFER_SIZE);
        ret = ssl_read(&ssl, tmpbuf, len);

        if (ret == POLARSSL_ERR_NET_WANT_READ || ret == POLARSSL_ERR_NET_WANT_WRITE)
            continue;

        if (ret == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY)
            break;

        if (ret < 0) {
            if (debug_level) fprintf(stderr, "failed\n  ! ssl_read returned %d\n\n", ret);
            break;
        }

        if (ret == 0) {
            if (debug_level) fprintf(stderr, "\n\nEOF\n\n");
            break;
        }

        len = ret;
        if (debug_level) fprintf(stderr, " %d bytes read\n\n%s\n", len, (char *) tmpbuf);
        strcpy(buf, tmpbuf);
    }
    while (1);

    ssl_close_notify(&ssl);

    exit:

#ifdef POLARSSL_ERROR_C
    if (ret != 0) {
        char error_buf[100];
        polarssl_strerror(ret, error_buf, 100);
        if (debug_level) fprintf(stderr, "Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif

    if (server_fd != -1)
        net_close(server_fd);

    x509_crt_free(&cacert);
    ssl_free(&ssl);
    ctr_drbg_free(&ctr_drbg);
    entropy_free(&entropy);

    memset(&ssl, 0, sizeof(ssl));

    return (buf);
}

#endif /* POLARSSL_BIGNUM_C && POLARSSL_ENTROPY_C && POLARSSL_SSL_TLS_C &&
          POLARSSL_SSL_CLI_C && POLARSSL_NET_C && POLARSSL_RSA_C &&
          POLARSSL_CTR_DRBG_C */
