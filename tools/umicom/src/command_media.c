/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_media.c
 *
 * PURPOSE:
 *   Expose existing setup and boot-media services without duplicating command policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "cli.h"
#include "umicom/boot_media/media.h"
#include "umicom/setup_centre/setup.h"
#ifdef UMICOM_CLI_SETUP_HAS_PROCESS
#include "umicom/setup_centre/execution.h"
#endif

/* These routes receive argv[0] as their command name. All parsing, target
 * admission, confirmation and output ownership remain in Framework services.
 * NativeArguments has already converted Windows arguments to UTF-8. */
int umi_cli_command_boot_media(int argc, char **argv)
{
    return UmiBootMediaMain(argc, argv);
}

int umi_cli_command_setup(int argc, char **argv)
{
#ifdef UMICOM_CLI_SETUP_HAS_PROCESS
    return UmiSetupNativeMain(argc, argv);
#else
    /* Reduced builds retain inspection and packaging; execution-dependent
     * operations report UNAVAILABLE through the existing setup contract. */
    return UmiSetupMain(argc, argv);
#endif
}
