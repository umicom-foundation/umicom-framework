/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/contract.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeContract *contract=umi_studio_runtime_contract();
    assert(contract!=NULL);
    assert(strcmp(contract->application_id,"umicom.studio")==0);
    assert(contract->required_surface_count==19U);
    assert(contract->required_alias_count==22U);
    assert(contract->required_studio_command_count==15U);
    assert(contract->required_layout_count==8U);
    return 0;
}
