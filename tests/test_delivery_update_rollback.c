/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_update_rollback.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/update.h"
#include "umicom/delivery/update_plan.h"
#include "umicom/delivery/rollback.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUpdatePlan update;
    UmiRollbackPlan rollback;
    assert(umi_update_decide(1U, 2U, 1, 1) == UMI_UPDATE_AVAILABLE);
    assert(umi_update_plan_init(&update, 1U, 2U, UMI_UPDATE_AVAILABLE, "new build") == UMI_STATUS_OK);
    assert(update.requires_restart);
    assert(umi_rollback_plan_init(&rollback, 2U, 1U, "health failed") == UMI_STATUS_OK);
    assert(umi_rollback_plan_approve(&rollback) == UMI_STATUS_OK);
    assert(umi_rollback_plan_valid(&rollback));
    return 0;
}
