/* Umicom Framework test fixture | Sammy Hegab | Umicom Foundation | MIT */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif
int main(int argc, char **argv)
{
    if (argc < 2) return 2;
    if (strcmp(argv[1], "fail") == 0) return 5;
    if (strcmp(argv[1], "skip") == 0) return 77;
    if (strcmp(argv[1], "wait") == 0 || strcmp(argv[1], "marker") == 0) {
        if (argc != 3) return 2;
        FILE *file = fopen(argv[2], "wb");
        if (file == NULL) return 3;
        (void)fputs("started\n", file);
        if (fclose(file) != 0) return 3;
        if (strcmp(argv[1], "wait") == 0) {
            /* Bounded fixture: independently surviving children still exit. */
#ifdef _WIN32
            Sleep(800U);
#else
            struct timespec duration = {0, 800000000L};
            (void)nanosleep(&duration, NULL);
#endif
        }
    }
    if (strcmp(argv[1], "large") == 0) {
        for (unsigned i = 0U; i < 140000U; ++i) (void)putchar('x');
        (void)puts("\nNOTES OUTPUT TAIL");
    }
    (void)puts(argv[1]);
    return strcmp(argv[1], "root=B") == 0 || strcmp(argv[1], "large") == 0 ? 5 : 0;
}
