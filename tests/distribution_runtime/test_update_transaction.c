/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_update_transaction.c
 *
 * PURPOSE:
 *   Focused regression coverage for update transaction lifecycle and commit/rollback state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_transaction.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrUpdateTransaction t; umi_dr_update_transaction_init(&t); t.checkpoint_ready=true; CHECK(umi_dr_update_transaction_stage(&t)==UMI_STATUS_OK); CHECK(umi_dr_update_transaction_commit(&t)==UMI_STATUS_INVALID_STATE); t.health_verified=true; CHECK(umi_dr_update_transaction_commit(&t)==UMI_STATUS_OK);
    return 0;
}
