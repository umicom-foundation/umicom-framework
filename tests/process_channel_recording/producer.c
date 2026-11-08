/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_channel_recording/producer.c
 *
 * PURPOSE:
 *   Emit deterministic binary stderr and separate stdout for native channel regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <time.h>
#include <errno.h>
#endif
static void Pause(void)
{
#ifdef _WIN32
    Sleep(30000U);
#else
    struct timespec delay = {30, 0};
    while (nanosleep(&delay, &delay) != 0 && errno == EINTR) {}
#endif
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
#ifdef _WIN32
    (void)_setmode(_fileno(stdout), _O_BINARY);
    (void)_setmode(_fileno(stderr), _O_BINARY);
#endif
    const char *mode = argv[1];
    if (strcmp(mode, "large") == 0) {
        unsigned char bytes[4096];
        for (size_t index = 0; index < sizeof bytes; ++index) bytes[index] = (unsigned char)index;
        /* Exceed both the diagnostic-tail capacity and ordinary pipe capacity.
         * The parent deliberately does not read or poll while this is written. */
        for (unsigned block = 0; block < 128U; ++block)
            if (fwrite(bytes, 1U, sizeof bytes, stderr) != sizeof bytes) return 3;
    } else if (strcmp(mode, "binary") == 0 || strcmp(mode, "wait") == 0 ||
               strcmp(mode, "exit") == 0) {
        const unsigned char bytes[] = {'e', 0U, 0xffU, '\r', '\n', 'z'};
        if (fwrite(bytes, 1U, sizeof bytes, stderr) != sizeof bytes) return 3;
    } else return 2;
    if (fflush(stderr) != 0) return 3;
    if (fwrite("protocol-only\n", 1U, 14U, stdout) != 14U || fflush(stdout) != 0) return 4;
    if (strcmp(mode, "wait") == 0) Pause();
    return strcmp(mode, "exit") == 0 ? 23 : 0;
}
