/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/setup_maintenance/main.c
 * PURPOSE:
 *   Thin native entry point; maintenance belongs to the shared installer core.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Thin native entry point; maintenance belongs to the shared installer core. */
#include "umicom/setup_centre/maintenance.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
int wmain(int argc, wchar_t **wide)
{
    char **args = calloc((size_t)argc + 1U, sizeof(*args));
    if (args == NULL) return 1;
    int status = 1;
    for (int i = 0; i < argc; ++i) {
        int count = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, NULL, 0, NULL, NULL);
        if (count <= 0) goto done;
        args[i] = malloc((size_t)count);
        if (args[i] == NULL || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, args[i], count, NULL, NULL) != count) goto done;
    }
    status = UmiSetupMaintenanceMain(argc, args);
done:
    for (int i = 0; i < argc; ++i) free(args[i]);
    free(args); return status;
}
#else
int main(int argc, char **argv) { return UmiSetupMaintenanceMain(argc, argv); }
#endif
