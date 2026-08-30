/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_audit_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository audit contract semantics.
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
    UmiRepositoryAudit *a = calloc(1U, sizeof(*a));
    assert(a != NULL); umi_repository_audit_init(a);
    assert(umi_repository_audit_append(a,UMI_REPOSITORY_CONTROL_INSPECTING,UMI_STATUS_OK,"","inspect")==UMI_STATUS_OK);
    assert(umi_repository_audit_latest(a)->sequence==1U);
    free(a);
    return 0;
}
