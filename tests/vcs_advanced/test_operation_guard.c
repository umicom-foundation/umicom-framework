/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_operation_guard.c
 *
 * PURPOSE:
 *   Validate describe preconditions that protect mutating source-control operations.
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
#include "umicom/vcs/advanced/operation_guard.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/operation_guard.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedOperationGuardTransferEqual(const UmiVcsAdvancedOperationGuard *a, const UmiVcsAdvancedOperationGuard *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        a->require_clean_worktree == b->require_clean_worktree &&
        a->require_no_conflicts == b->require_no_conflicts &&
        a->require_upstream == b->require_upstream &&
        a->require_no_unpushed_commits == b->require_no_unpushed_commits &&
        a->allow_detached_head == b->allow_detached_head &&
        a->safety == b->safety;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedOperationGuardTransferTails(UmiVcsAdvancedOperationGuard *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedOperationGuardTransferMalformed(const UmiVcsAdvancedOperationGuard *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedOperationGuardTransferCases, UmiVcsAdvancedOperationGuard,
    umi_vcs_advanced_operation_guard_archive_encode, umi_vcs_advanced_operation_guard_archive_decode,
    UmiVcsAdvancedOperationGuardTransferEqual, UmiVcsAdvancedOperationGuardTransferTails, UmiVcsAdvancedOperationGuardTransferMalformed)

int main(void)
{
    UmiVcsAdvancedOperationGuard value;
    umi_vcs_advanced_operation_guard_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_operation_guard_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedOperationGuardTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_operation_guard_allows(&value, 1, 0, 1, 0, 0)) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_operation_guard_allows(&value, 1, 1, 1, 0, 0)) return 4;
    return 0;
}
