/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_release_controls.c
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
#include "umicom/delivery/promotion.h"
#include "umicom/delivery/release_lock.h"
#include "umicom/delivery/rollback_history.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPromotion promotion;
    UmiReleaseLock lock;
    UmiRollbackHistory history;
    UmiRollbackPlan rollback;
    assert(umi_promotion_init(&promotion, "r", UMI_RELEASE_BETA, UMI_RELEASE_STABLE) == UMI_STATUS_OK);
    assert(umi_promotion_direction_valid(&promotion));
    umi_release_lock_init(&lock);
    assert(umi_release_lock_acquire(&lock, "studio") == UMI_STATUS_OK);
    assert(umi_release_lock_release(&lock, "studio") == UMI_STATUS_OK);
    umi_rollback_history_init(&history);
    assert(umi_rollback_plan_init(&rollback, 2U, 1U, "test") == UMI_STATUS_OK);
    assert(umi_rollback_history_add(&history, &rollback) == UMI_STATUS_OK);
    return 0;
}
