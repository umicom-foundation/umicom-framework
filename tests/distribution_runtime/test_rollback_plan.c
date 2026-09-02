/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_rollback_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for rollback checkpoint and prior-version restoration policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/rollback_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrRollbackPlan value; umi_dr_rollback_plan_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"rb")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.checkpoint_id,sizeof(value.checkpoint_id),"cp")==UMI_STATUS_OK); value.verified=true; CHECK(umi_dr_rollback_plan_valid(&value)); CHECK(umi_dr_rollback_plan_fingerprint(&value) != 0U);
    return 0;
}
