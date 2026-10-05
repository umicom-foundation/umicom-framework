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
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/distribution/runtime/rollback_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/rollback_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrRollbackPlanTransferEqual(const UmiDrRollbackPlan *a, const UmiDrRollbackPlan *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->restore_version.major == b->restore_version.major &&
        a->restore_version.minor == b->restore_version.minor &&
        a->restore_version.patch == b->restore_version.patch &&
        strcmp(a->checkpoint_id, b->checkpoint_id) == 0 &&
        a->preserve_user_data == b->preserve_user_data &&
        a->verified == b->verified;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrRollbackPlanTransferTails(UmiDrRollbackPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->checkpoint_id) + 1U;
        memset(value->checkpoint_id + used, 0xa5, sizeof(value->checkpoint_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrRollbackPlanTransferMalformed(const UmiDrRollbackPlan *sample)
{
    (void)sample;
    {
        UmiDrRollbackPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_rollback_plan_valid(&invalid)) ||
            umi_dr_rollback_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrRollbackPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.checkpoint_id, 'x', sizeof(invalid.checkpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_rollback_plan_valid(&invalid)) ||
            umi_dr_rollback_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated checkpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrRollbackPlanTransferCases, UmiDrRollbackPlan,
    umi_dr_rollback_plan_archive_encode, umi_dr_rollback_plan_archive_decode,
    UmiDrRollbackPlanTransferEqual, UmiDrRollbackPlanTransferTails, UmiDrRollbackPlanTransferMalformed)

int main(void) {
    UmiDrRollbackPlan value; umi_dr_rollback_plan_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"rb")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.checkpoint_id,sizeof(value.checkpoint_id),"cp")==UMI_STATUS_OK); value.verified=true; CHECK(umi_dr_rollback_plan_valid(&value));
    if (UmiDrRollbackPlanTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_rollback_plan_fingerprint(&value) != 0U);
    return 0;
}
