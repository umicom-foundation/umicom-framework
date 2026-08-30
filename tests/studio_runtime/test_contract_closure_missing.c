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

#include <assert.h>
#include "umicom/studio_runtime/closure.h"
int main(void)
{
    UmiApplicationShellRegistry *shell=NULL;
    UmiCommandRegistry *commands=NULL;
    UmiStudioRuntimeClosureReport report;
    assert(umi_application_shell_registry_create(&shell)==UMI_STATUS_OK);
    assert(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    assert(umi_studio_runtime_closure_check(
        umi_studio_runtime_contract(),shell,commands,&report)==UMI_STATUS_OK);
    assert(!report.ready);
    assert(report.missing_surface_count>0U);
    assert(report.missing_command_count>0U);
    umi_command_registry_destroy(commands);
    umi_application_shell_registry_destroy(shell);
    return 0;
}

