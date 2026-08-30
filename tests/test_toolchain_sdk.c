/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain_sdk.c
 *
 * PURPOSE:
 *   Implement the test toolchain sdk behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework - Batch 35 SDK metadata regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/toolchain/sdk.h"

int main(void)
{
    UmiToolchainProfile profile;
    UmiSdkCatalogueSnapshot catalogue;
    umi_toolchain_profile_init(&profile);
    profile.family = UMI_TOOLCHAIN_POSIX_GCC;
    strcpy(profile.prefix_directory, "/usr");
    if (umi_toolchain_sdk_discover(&profile, &catalogue) != UMI_STATUS_OK ||
        catalogue.count != 1U ||
        catalogue.items[0].kind != UMI_SDK_POSIX ||
        strcmp(catalogue.items[0].id, "posix-system") != 0) return 1;
    return EXIT_SUCCESS;
}
