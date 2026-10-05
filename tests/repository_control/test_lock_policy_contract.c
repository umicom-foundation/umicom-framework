/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_lock_policy_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository lock policy contract semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
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
#include <assert.h>
#include "umicom/repository/lock_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/repository/lock_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRepositoryLockPolicyTransferEqual(const UmiRepositoryLockPolicy *a, const UmiRepositoryLockPolicy *b)
{
    return a->dry_run == b->dry_run &&
        a->stage_gitlinks == b->stage_gitlinks &&
        a->require_all_heads == b->require_all_heads &&
        a->require_clean_parent == b->require_clean_parent &&
        a->verify_after_stage == b->verify_after_stage;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRepositoryLockPolicyTransferTails(UmiRepositoryLockPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRepositoryLockPolicyTransferMalformed(const UmiRepositoryLockPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRepositoryLockPolicyTransferCases, UmiRepositoryLockPolicy,
    umi_repository_lock_policy_archive_encode, umi_repository_lock_policy_archive_decode,
    UmiRepositoryLockPolicyTransferEqual, UmiRepositoryLockPolicyTransferTails, UmiRepositoryLockPolicyTransferMalformed)

int main(void)
{
    UmiRepositoryLockPolicy p;
    umi_repository_lock_policy_init(&p);
    assert(p.stage_gitlinks);
    assert(p.require_all_heads);
    assert(umi_repository_lock_policy_validate(&p) == UMI_STATUS_OK);
    if (UmiRepositoryLockPolicyTransferCases(&p) != 0) return 1;

    return 0;
}
