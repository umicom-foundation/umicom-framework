/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_qemu.c
 * PURPOSE: Delegate the native QEMU command to the shared boot service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/vm_manager/boot.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#endif

/* The native command entry point now captures Unicode arguments in Framework for all commands. The former QEMU-only conversion is retained for review; this adapter forwards the already converted argument slice. The previous implementation is retained for engineering review. */
#if 0
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
#endif
/* Compose the existing QEMU entry point with Framework's native Kernel
 * qualification. No Kernel code links back into the Framework CLI. */
int umi_cli_command_kernel_qualification(int argc, char **argv);
int umi_cli_command_qemu(int argc, char **argv)
{
    if (argc > 0 && argv && argv[0] &&
        strcmp(argv[0], "kernel-qualify") == 0)
        return umi_cli_command_kernel_qualification(argc - 1, argv + 1);
    /* Keep the existing boot help and add a discoverable qualification entry. */
    if (argc == 0 || (argc == 1 && argv && argv[0] &&
        (strcmp(argv[0], "help") == 0 || strcmp(argv[0], "--help") == 0)))
        (void)puts("Also: umicom qemu kernel-qualify --help");
    /* The shared native entry point already owns the original argument vector.
     * QEMU routing therefore follows the same Unicode boundary as repository and build commands. */
    return UmiVmBootMain(argc, argv);
}
