/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_stash_plan.c
 *
 * PURPOSE:
 *   Validate plan stash push/apply/pop/drop/branch operations with explicit conflict and index intent.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
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
#include "umicom/vcs/advanced/stash_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/stash_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedStashPlanTransferEqual(const UmiVcsAdvancedStashPlan *a, const UmiVcsAdvancedStashPlan *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->action == b->action &&
        strcmp(a->stash_ref, b->stash_ref) == 0 &&
        strcmp(a->message, b->message) == 0 &&
        strcmp(a->branch_name, b->branch_name) == 0 &&
        a->include_untracked == b->include_untracked &&
        a->keep_index == b->keep_index &&
        a->reinstate_index == b->reinstate_index &&
        a->safety == b->safety;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedStashPlanTransferTails(UmiVcsAdvancedStashPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->stash_ref) + 1U;
        memset(value->stash_ref + used, 0xa5, sizeof(value->stash_ref) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
    {
        size_t used = strlen(value->branch_name) + 1U;
        memset(value->branch_name + used, 0xa5, sizeof(value->branch_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedStashPlanTransferMalformed(const UmiVcsAdvancedStashPlan *sample)
{
    (void)sample;
    {
        UmiVcsAdvancedStashPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.stash_ref, 'x', sizeof(invalid.stash_ref));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_stash_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_stash_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated stash_ref was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedStashPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_stash_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_stash_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedStashPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.branch_name, 'x', sizeof(invalid.branch_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_stash_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_stash_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated branch_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedStashPlanTransferCases, UmiVcsAdvancedStashPlan,
    umi_vcs_advanced_stash_plan_archive_encode, umi_vcs_advanced_stash_plan_archive_decode,
    UmiVcsAdvancedStashPlanTransferEqual, UmiVcsAdvancedStashPlanTransferTails, UmiVcsAdvancedStashPlanTransferMalformed)

int main(void)
{
    UmiVcsAdvancedStashPlan p; umi_vcs_advanced_stash_plan_init(&p);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_plan_push(&p,"checkpoint",1,0)!=UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_plan_validate(&p)!=UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedStashPlanTransferCases(&p) != 0) return 1;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_plan_apply(&p,"stash@{0}",1,1)!=UMI_STATUS_OK) return 3;
    return 0;
}
