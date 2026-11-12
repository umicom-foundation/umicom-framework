/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/server_fixture.c
 *
 * PURPOSE:
 *   Act as a finite owned child for process-lifetime regression tests without loading a model or opening a socket.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/threading.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 19 || strcmp(argv[1], "--model") != 0 || strcmp(argv[3], "--host") != 0 ||
        strcmp(argv[4], "127.0.0.1") != 0 || strcmp(argv[11], "--n-gpu-layers") != 0 ||
        strcmp(argv[12], "0") != 0 || strcmp(argv[13], "--parallel") != 0 ||
        strcmp(argv[14], "1") != 0 || strcmp(argv[17], "--offline") != 0 ||
        strcmp(argv[18], "--no-webui") != 0) return 2;
    (void)puts("Owned fixture started; no model inference or HTTP service.");
    (void)fflush(stdout);
    /* A hard bound prevents a failed parent test from leaving a perpetual
     * fixture. Normal owner-run coverage stops the child well before this. */
    umi_thread_sleep_ms(10000U);
    return 0;
}
