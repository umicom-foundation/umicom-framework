/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/boot_peer.c
 * PURPOSE: Provide an inert portable QMP peer for lifecycle tests; never execute a guest.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    /* Firmware basenames select fixture responses. This child is intentionally
     * not QEMU: it tests pipe ownership, protocol handling and shutdown only. */
    int running = 0, malformed = 0;
    for (int index = 1; index + 1 < argc; ++index) {
        if (strcmp(argv[index], "-bios") == 0) {
            const char *path = argv[index + 1];
            const char *base = strrchr(path, '/');
            base = base ? base + 1 : path;
            running = strcmp(base, "running.fw") == 0;
            malformed = strcmp(base, "malformed.fw") == 0;
        }
    }
    if (malformed) {
        puts("{\"QMP\":false}");
        fflush(stdout);
        return 0;
    }
    puts("{\"QMP\":{\"version\":{\"qemu\":{\"major\":10,\"minor\":0,\"micro\":0},"
         "\"package\":\"inert boot peer\"},\"capabilities\":[]}}");
    fflush(stdout);
    char line[8192];
    while (fgets(line, sizeof line, stdin)) {
        const char *id = strstr(line, "\"id\":");
        if (!id) return 2;
        char *end = NULL;
        unsigned long number = strtoul(id + 5, &end, 10);
        if (end == id + 5) return 2;
        if (strstr(line, "\"cont\"")) running = 1;
        if (strstr(line, "\"stop\"")) running = 0;
        if (strstr(line, "query-status"))
            printf("{\"return\":{\"running\":%s,\"status\":\"%s\"},\"id\":%lu}\n",
                running ? "true" : "false", running ? "running" : "paused", number);
        else if (strstr(line, "ringbuf-read"))
            printf("{\"return\":\"Z3Vlc3QgZml4dHVyZQo=\",\"id\":%lu}\n", number);
        else printf("{\"return\":{},\"id\":%lu}\n", number);
        fflush(stdout);
        if (strstr(line, "\"quit\"")) return 0;
    }
    return 0;
}
