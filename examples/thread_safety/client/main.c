/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/thread_safety/client/main.c
 * PURPOSE:
 *   Exercise the installed thread-safety API from an independent SDK consumer.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include <umicom/platform/threading.h>
#include <stdio.h>
static int Work(void *unused) { (void)unused; return 23; }
int main(void)
{
    UmiThread *thread = NULL; int result = 0;
    if (umi_thread_start(Work, NULL, &thread) != UMI_STATUS_OK) return 1;
    if (umi_thread_join(thread, &result) != UMI_STATUS_OK) return 2;
    if (UmiThreadTryGetExitCode(thread, &result) != UMI_STATUS_OK || result != 23) return 3;
    if (UmiThreadRelease(&thread) != UMI_STATUS_OK || thread != NULL) return 4;
    puts("Installed Framework thread contract returned 23 and released its handle.");
    return 0;
}
