/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_test_output/child.c
 * PURPOSE: Provide bounded child output and a release handshake for real CTest capture checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

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
static void pause_briefly(void)
{
#ifdef _WIN32
    Sleep(10U);
#else
    struct timespec delay = {0, 10000000L};
    (void)nanosleep(&delay, NULL);
#endif
}
int main(int argc, char **argv)
{
    if (argc < 2)
        return 2;
    if (strcmp(argv[1], "slow") == 0)
    {
        if (argc != 3)
            return 2;
        puts("LIVE OUTPUT BEGIN");
        fflush(stdout);
        /* The parent releases only after reading the still-running capture.
         * Bound the wait even when the parent assertion fails. */
        for (unsigned poll = 0U; poll < 1000U; ++poll)
        {
            FILE *release = fopen(argv[2], "rb");
            if (release != NULL)
            {
                fclose(release);
                puts("LIVE OUTPUT FINISHED");
                return 0;
            }
            pause_briefly();
        }
        return 6;
    }
    if (strcmp(argv[1], "large") == 0)
    {
        for (unsigned line = 0U; line < 4000U; ++line)
            puts("Large diagnostic line retained only within the selected tail capacity.");
        puts("LARGE OUTPUT END");
    }
    else if (strcmp(argv[1], "unicode") == 0)
    {
        puts("caf\xc3\xa9 \xe6\xb5\x8b\xe8\xaf\x95");
    }
    else
        puts("SMALL OUTPUT END");
    return strcmp(argv[1], "fail") == 0 ? 5 : strcmp(argv[1], "skip") == 0 ? 77 : 0;
}
