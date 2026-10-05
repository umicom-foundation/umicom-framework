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

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "umicom/delivery/update.h"
#include "umicom/delivery/update_plan.h"
#include "umicom/delivery/rollback.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/rollback.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRollbackPlanTransferEqual(const UmiRollbackPlan *a, const UmiRollbackPlan *b)
{
    return a->current_generation == b->current_generation &&
        a->target_generation == b->target_generation &&
        strcmp(a->reason, b->reason) == 0 &&
        a->approved == b->approved;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRollbackPlanTransferTails(UmiRollbackPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->reason) + 1U;
        memset(value->reason + used, 0xa5, sizeof(value->reason) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRollbackPlanTransferMalformed(const UmiRollbackPlan *sample)
{
    (void)sample;
    {
        UmiRollbackPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.reason, 'x', sizeof(invalid.reason));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rollback_plan_valid(&invalid)) ||
            umi_rollback_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated reason was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRollbackPlanTransferCases, UmiRollbackPlan,
    umi_rollback_plan_archive_encode, umi_rollback_plan_archive_decode,
    UmiRollbackPlanTransferEqual, UmiRollbackPlanTransferTails, UmiRollbackPlanTransferMalformed)

int main(void) {
    UmiUpdatePlan update;
    UmiRollbackPlan rollback;
    assert(umi_update_decide(1U, 2U, 1, 1) == UMI_UPDATE_AVAILABLE);
    assert(umi_update_plan_init(&update, 1U, 2U, UMI_UPDATE_AVAILABLE, "new build") == UMI_STATUS_OK);
    assert(update.requires_restart);
    assert(umi_rollback_plan_init(&rollback, 2U, 1U, "health failed") == UMI_STATUS_OK);
    assert(umi_rollback_plan_approve(&rollback) == UMI_STATUS_OK);
    assert(umi_rollback_plan_valid(&rollback));
    if (UmiRollbackPlanTransferCases(&rollback) != 0) return 1;

    return 0;
}
