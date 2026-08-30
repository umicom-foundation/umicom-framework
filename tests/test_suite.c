/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_suite.c
 *
 * PURPOSE:
 *   Verify independent application registration and suite manifest generation
 *   without merging product-specific implementation code into Framework Core.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiSuite suite;
    UmiSuiteApplication studio = {
        "org.umicom.studio", "Umicom Studio IDE", "applications/studio",
        "umicom-studio-ide", 1
    };
    UmiSuiteApplication trader = {
        "org.umicom.trader", "Umicom Trader", "applications/trader",
        "umicom-trader", 1
    };
    char message[256];
    char temporary[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];

    umi_suite_init(&suite, "org.umicom.suite", "Umicom Suite");
    if (umi_suite_add(&suite, &studio) != UMI_STATUS_OK ||
        umi_suite_add(&suite, &trader) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_suite_validate(&suite, message, sizeof(message)) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_suite_find(&suite, "org.umicom.trader") == NULL)
        return EXIT_FAILURE;
    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK ||
        umi_fs_join(path,
                    sizeof(path),
                    temporary,
                    "umicom-suite-test.yaml") != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_suite_write_manifest(&suite, path) != UMI_STATUS_OK ||
        !umi_fs_is_file(path))
        return EXIT_FAILURE;
    (void)remove(path);
    return EXIT_SUCCESS;
}
