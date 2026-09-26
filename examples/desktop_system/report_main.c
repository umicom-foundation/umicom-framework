/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/desktop_system/report_main.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    UmiDesktopSystemOptions options = {0};
    for (int i = 1; i < argc; ++i) {
        if (i + 1 >= argc) { fputs("Missing value.\n", stderr); return 2; }
        if (!strcmp(argv[i], "--storage")) options.storagePath = argv[++i];
        else if (!strcmp(argv[i], "--boot-report")) options.bootReportPath = argv[++i];
        else if (!strcmp(argv[i], "--fixture-proc")) options.linuxProcRoot = argv[++i];
        else { fputs("Usage: umicom-system-report [--storage absolute-path] [--boot-report absolute-file] [--fixture-proc absolute-directory]\n", stderr); return 2; }
    }
    UmiDesktopSystemSnapshot *snapshot = malloc(sizeof *snapshot);
    if (!snapshot) return 1;
    UmiStatus status = UmiDesktopSystemCapture(&options, snapshot); size_t required = 0;
    if (status == UMI_STATUS_OK) status = UmiDesktopSystemJson(snapshot, NULL, 0, &required);
    char *json = status == UMI_STATUS_OK ? malloc(required) : NULL;
    if (!json) { free(snapshot); fputs("Capture or allocation failed.\n", stderr); return 1; }
    status = UmiDesktopSystemJson(snapshot, json, required, &required); free(snapshot);
    int result = status != UMI_STATUS_OK || fputs(json, stdout) == EOF || fflush(stdout) != 0;
    free(json); return result;
}
