/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_contract_closure_missing.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime contract closure missing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdlib.h>

#include "umicom/test_runtime/check.h"
#include "umicom/studio_runtime/closure.h"
int main(void)
{
    UmiApplicationShellRegistry *shell=NULL;
    UmiCommandRegistry *commands=NULL;
    UmiStudioRuntimeClosureReport *report =
        (UmiStudioRuntimeClosureReport *)calloc(1U, sizeof(*report));
    UMI_TEST_REQUIRE(report != NULL);
    UMI_TEST_REQUIRE(umi_application_shell_registry_create(&shell)==UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_studio_runtime_closure_check(
        umi_studio_runtime_contract(),shell,commands,report)==UMI_STATUS_OK);
    UMI_TEST_REQUIRE(!report->ready);
    UMI_TEST_REQUIRE(report->missing_surface_count>0U);
    UMI_TEST_REQUIRE(report->missing_command_count>0U);
    umi_command_registry_destroy(commands);
    umi_application_shell_registry_destroy(shell);
    free(report);
    return 0;
}
