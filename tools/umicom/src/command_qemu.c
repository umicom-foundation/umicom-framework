/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_qemu.c
 * PURPOSE: Delegate the native QEMU command to the shared boot service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/vm_manager/boot.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#endif

int umi_cli_command_qemu(int argc, char **argv)
{
#ifdef _WIN32
    /* The existing main entry point receives the Windows legacy code page.
     * Read the original wide arguments for this command so Unicode image paths
     * reach Framework unchanged. Other command entry points are retained. */
    (void)argc;
    (void)argv;
    int wideCount = 0;
    wchar_t **wide = CommandLineToArgvW(GetCommandLineW(), &wideCount);
    if (!wide || wideCount < 2) {
        if (wide) LocalFree(wide);
        return 1;
    }
    size_t count = (size_t)(wideCount - 2);
    char **utf8 = calloc(count + 1U, sizeof *utf8);
    int result = 1;
    if (utf8) {
        for (size_t index = 0; index < count; ++index) {
            int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                wide[index + 2U], -1, NULL, 0, NULL, NULL);
            if (bytes <= 0) goto done;
            utf8[index] = malloc((size_t)bytes);
            if (!utf8[index] || !WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                wide[index + 2U], -1, utf8[index], bytes, NULL, NULL)) goto done;
        }
        result = UmiVmBootMain(wideCount - 2, utf8);
    }
done:
    if (utf8) {
        for (size_t index = 0; index < count; ++index) free(utf8[index]);
        free(utf8);
    }
    LocalFree(wide);
    return result;
#else
    return UmiVmBootMain(argc, argv);
#endif
}
