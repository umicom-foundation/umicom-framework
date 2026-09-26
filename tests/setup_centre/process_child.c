/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
  * An inert native child. It never starts another process or writes a file. */
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
#include <unistd.h>
#endif
int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "sleep") == 0) {
        puts("inert child started");
        fflush(stdout);
#ifdef _WIN32
        Sleep(10000U);
#else
        struct timespec pause = {10, 0};
        (void)nanosleep(&pause, NULL);
#endif
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "fail") == 0) {
        fputs("inert child deliberate failure\n", stderr);
        return 13;
    }
    for (int i = 1; i < argc; ++i) printf("[%s]\n", argv[i]);
    return 0;
}
