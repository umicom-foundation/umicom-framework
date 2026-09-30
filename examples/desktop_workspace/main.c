/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/desktop_workspace/main.c
 * PURPOSE:
 *   Native Windows entry preserves Unicode arguments across the public UTF-8 ABI. The
 *   command implementation remains the same on Windows and Linux.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native Windows entry preserves Unicode arguments across the public UTF-8 ABI.
 * The command implementation remains the same on Windows and Linux. */
#include "umicom/desktop_workspace/workspace.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
int wmain(int argc, wchar_t **wide)
{
    if (argc <= 0 || argc > 64) return 2;
    char **argv = calloc((size_t)argc + 1U, sizeof *argv);
    if (!argv) return 1;
    int result = 1;
    for (int i = 0; i < argc; ++i) {
        int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, NULL, 0, NULL, NULL);
        if (size <= 0 || size > 65536) goto finish;
        argv[i] = malloc((size_t)size);
        if (!argv[i] || !WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide[i], -1, argv[i], size, NULL, NULL)) goto finish;
    }
    result = UmiDesktopWorkspaceMain(argc, argv);
finish:
    for (int i = 0; i < argc; ++i) free(argv[i]);
    free(argv); return result;
}
#else
int main(int argc, char **argv) { return UmiDesktopWorkspaceMain(argc, argv); }
#endif
