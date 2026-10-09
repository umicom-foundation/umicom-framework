/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/tool_location/process_probe.c
 * PURPOSE: Expose the exact child environment and literal argument used by the native tool test.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/process_search_path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* The native fixture must read Windows arguments as UTF-8 so selected tool folders containing non-ASCII characters are checked without the active ANSI code page changing them. The previous implementation is retained for engineering review. */
#if 0
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    char *path = NULL;
    /* The shared capture reads Unicode PATH natively on Windows. Remove only
     * the prefix that this probe just added to expose its received environment. */
    if (UmiProcessSearchPathCapture(argv[1], &path) != UMI_STATUS_OK)
        return 3;
    size_t prefix = strlen(argv[1]);
    const char *received = path + prefix;
    if (*received != '\0')
        ++received;
    printf("PATH=[%s]\nARG=[%s]\nMARKER=[%s]\n", received, argv[2],
           getenv("UMICOM_TOOL_PROBE") != NULL ? getenv("UMICOM_TOOL_PROBE") : "");
    UmiProcessSearchPathFree(path);
    return 0;
}
#endif
static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    char *path = NULL;
    /* The shared capture reads Unicode PATH natively on Windows. Remove only
     * the prefix that this probe just added to expose its received environment. */
    if (UmiProcessSearchPathCapture(argv[1], &path) != UMI_STATUS_OK)
        return 3;
    size_t prefix = strlen(argv[1]);
    const char *received = path + prefix;
    if (*received != '\0')
        ++received;
    printf("PATH=[%s]\nARG=[%s]\nMARKER=[%s]\n", received, argv[2],
           getenv("UMICOM_TOOL_PROBE") != NULL ? getenv("UMICOM_TOOL_PROBE") : "");
    UmiProcessSearchPathFree(path);
    return 0;
}

#include "../native_process/utf8_entry.inc"
