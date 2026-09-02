/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_audit_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository audit boundary semantics.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryAudit *a = calloc(1U, sizeof(*a)); size_t n;
    assert(a != NULL); umi_repository_audit_init(a);
    /* Visit each bounded item once so every record receives the same rule. */
    for(n=0U;n<UMI_REPOSITORY_CONTROL_EVENT_CAPACITY+2U;++n)
     assert(umi_repository_audit_append(a,UMI_REPOSITORY_CONTROL_INSPECTING,UMI_STATUS_OK,"","x")==UMI_STATUS_OK);
    assert(a->count==UMI_REPOSITORY_CONTROL_EVENT_CAPACITY);
    assert(umi_repository_audit_latest(a)->sequence==UMI_REPOSITORY_CONTROL_EVENT_CAPACITY+2U);
    free(a);
    return 0;
}
