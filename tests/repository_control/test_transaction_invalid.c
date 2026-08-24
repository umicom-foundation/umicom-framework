/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_transaction_invalid.c
 *
 * PURPOSE:
 *   Regression coverage for repository transaction invalid semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include "umicom/repository/transaction.h"

int main(void)
{
    UmiRepositoryTransaction *t = calloc(1U, sizeof(*t));
    assert(t != NULL); umi_repository_transaction_init(t);
    assert(umi_repository_transaction_add(t,(UmiRepositoryTransactionAction)99,"x")==UMI_STATUS_INVALID_ARGUMENT);
    free(t);
    return 0;
}
