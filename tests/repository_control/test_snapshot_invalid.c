/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_snapshot_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository snapshot invalid semantics.
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
#include "umicom/repository/snapshot.h"

int main(void)
{
    assert(umi_repository_snapshot_from_plan(UMI_REPOSITORY_CONTROL_IDLE,
     UMI_STATUS_OK, NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
