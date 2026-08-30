/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_audit_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository audit invalid semantics.
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
#include <stdlib.h>
#include "umicom/repository/audit.h"

int main(void)
{
    assert(umi_repository_audit_append(NULL,UMI_REPOSITORY_CONTROL_IDLE,UMI_STATUS_OK,"","x")==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
