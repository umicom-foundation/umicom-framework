/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_execution/producer.c
 * PURPOSE: Provide deterministic command output and deadlines for supervised terminal tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* No network, user files, or interactive terminal are involved in this producer. */
int main(int argc, char **argv)
{
    if (argc < 2)
        return 2;
    const char *value = getenv("UMICOM_TERMINAL_FIXTURE");
    printf("ready:%s\n", value != NULL ? value : "(unset)");
    if (argc > 2)
        printf("argument:%s\n", argv[2]);
    fflush(stdout);
    if (strcmp(argv[1], "hold") == 0)
    {
        UmiClock clock = umi_clock_system();
        (void)clock.sleep_milliseconds(&clock, 20000U);
        umi_clock_dispose(&clock);
    }
    else if (strcmp(argv[1], "large") == 0)
    {
        for (unsigned index = 0U; index < 9000U; ++index)
            fputs("ordinary terminal output with a bounded retention window\n", stdout);
    }
    fputs("last:caf\xc3\xa9\n", stdout);
    return strcmp(argv[1], "fail") == 0 ? 9 : 0;
}
