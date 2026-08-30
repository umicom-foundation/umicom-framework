/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_control_types_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository control types contract semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
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
#include "umicom/repository/control_types.h"

int main(void)
{
    assert(strcmp(umi_repository_control_state_text(UMI_REPOSITORY_CONTROL_PLANNED), "planned") == 0);
    assert(strcmp(umi_repository_control_severity_text(UMI_REPOSITORY_CONTROL_ERROR), "error") == 0);
    return 0;
}
