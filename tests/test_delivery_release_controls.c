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

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "value_archive/construction_cases.h"
#include "value_archive/construction_cases.h"
#include "umicom/delivery/promotion.h"
#include "umicom/delivery/release_lock.h"
#include "umicom/delivery/rollback_history.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/promotion.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPromotionTransferEqual(const UmiPromotion *a, const UmiPromotion *b)
{
    return strcmp(a->release_id, b->release_id) == 0 &&
        a->from_channel == b->from_channel &&
        a->to_channel == b->to_channel &&
        a->status == b->status;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPromotionTransferTails(UmiPromotion *value)
{
    (void)value;
    {
        size_t used = strlen(value->release_id) + 1U;
        memset(value->release_id + used, 0xa5, sizeof(value->release_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPromotionTransferMalformed(const UmiPromotion *sample)
{
    (void)sample;
    {
        UmiPromotion invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.release_id, 'x', sizeof(invalid.release_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_promotion_direction_valid(&invalid)) ||
            umi_promotion_direction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated release_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPromotionTransferCases, UmiPromotion,
    umi_promotion_direction_archive_encode, umi_promotion_direction_archive_decode,
    UmiPromotionTransferEqual, UmiPromotionTransferTails, UmiPromotionTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiPromotionTransferCasesWithWorkspace, UmiPromotion,
    umi_promotion_direction_archive_encode, umi_promotion_direction_archive_decode_with_workspace, UmiPromotionTransferEqual)

#include "value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiPromotionConditionalArchiveCases, UmiPromotion,
    umi_promotion_direction_archive_encode, umi_promotion_direction_archive_decode_if_unchanged,
    UmiPromotionTransferEqual, UmiPromotionTransferTails, UmiPromotionTransferMalformed)



/* A rejected replacement must preserve the previously accepted promotion value. */
static int UmiPromotionConstructionCases(void)
{
    UmiPromotion *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_promotion_init(value, "r", UMI_RELEASE_BETA, UMI_RELEASE_STABLE) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->release_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_promotion_init(value, oversized, UMI_RELEASE_BETA, UMI_RELEASE_STABLE) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_promotion_init(value, value->release_id, UMI_RELEASE_BETA, UMI_RELEASE_STABLE) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->release_id, "r") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}


/* A rejected replacement must preserve the previously accepted rollback value. */
static int UmiRollbackPlanConstructionCases(void)
{
    UmiRollbackPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_rollback_plan_init(value, 2U, 1U, "test") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->reason) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_rollback_plan_init(value, 2U, 1U, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_rollback_plan_init(value, 2U, 1U, value->reason) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->reason, "test") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiRollbackPlanConstructionCases() != 0) return 1;

    if (UmiPromotionConstructionCases() != 0) return 1;

    UmiPromotion promotion;
    UmiReleaseLock lock;
    UmiRollbackHistory history;
    UmiRollbackPlan rollback;
    assert(umi_promotion_init(&promotion, "r", UMI_RELEASE_BETA, UMI_RELEASE_STABLE) == UMI_STATUS_OK);
    assert(umi_promotion_direction_valid(&promotion));
    if (UmiPromotionTransferCases(&promotion) != 0) return 1;
    if (UmiPromotionTransferCasesWithWorkspace(&promotion) != 0) return 1;
    if (UmiPromotionConditionalArchiveCases(&promotion) != 0) return 1;

    umi_release_lock_init(&lock);
    assert(umi_release_lock_acquire(&lock, "studio") == UMI_STATUS_OK);
    assert(umi_release_lock_release(&lock, "studio") == UMI_STATUS_OK);
    umi_rollback_history_init(&history);
    assert(umi_rollback_plan_init(&rollback, 2U, 1U, "test") == UMI_STATUS_OK);
    assert(umi_rollback_history_add(&history, &rollback) == UMI_STATUS_OK);
    return 0;
}
