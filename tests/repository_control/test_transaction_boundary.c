/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_transaction_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository transaction boundary semantics.
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
#include "umicom/repository/transaction.h"

int main(void)
{
    UmiRepositoryTransaction *t = calloc(1U, sizeof(*t));
    assert(t != NULL); umi_repository_transaction_init(t);
    assert(umi_repository_transaction_add(t,UMI_REPOSITORY_TRANSACTION_STAGE_GITLINK,"framework")==UMI_STATUS_OK);
    assert(umi_repository_transaction_mark(t,0U,UMI_REPOSITORY_TRANSACTION_FAILED,UMI_STATUS_INVALID_STATE)==UMI_STATUS_OK);
    assert(!umi_repository_transaction_successful(t));
    free(t);
    return 0;
}
