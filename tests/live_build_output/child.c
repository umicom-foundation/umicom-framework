/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_build_output/child.c
 * PURPOSE: Emit inert child-process output and await an observer acknowledgement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/threading.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    (void)fputs("ready\n", stdout); (void)fflush(stdout);
    if (strcmp(argv[1], "unicode") == 0) {
        const char raw[] = {'c', 'a', 'f', '\xc3', '\xa9', '\0', 'x'};
        (void)fwrite(raw, 1U, sizeof(raw), stdout); (void)fflush(stdout);
    }
    (void)fputs("notes.c:2:3: warning: fixture diagnostic\n", stderr); (void)fflush(stderr);
    if (strcmp(argv[1], "legacy") == 0) return 0;
    if (strcmp(argv[1], "failure") == 0) return 5;
    /* A missing observer would time out: successful streaming must acknowledge
     * bytes while this process is alive, not replay them after process exit. */
    for (unsigned i = 0U; i < 10000U; ++i) {
        FILE *ack = fopen("stream-ack", "rb");
        if (ack != NULL) { (void)fclose(ack); return 0; }
        umi_thread_sleep_ms(1U);
    }
    return 6;
}
