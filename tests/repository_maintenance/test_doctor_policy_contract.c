/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_maintenance/test_doctor_policy_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for repository maintenance module doctor_policy.
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
#include "umicom/repository/doctor_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/repository/doctor_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRepositoryDoctorPolicyTransferEqual(const UmiRepositoryDoctorPolicy *a, const UmiRepositoryDoctorPolicy *b)
{
    return a->allow_dirty_worktree == b->allow_dirty_worktree &&
        a->require_origin == b->require_origin &&
        a->require_upstream == b->require_upstream &&
        a->require_initialised_submodules == b->require_initialised_submodules &&
        a->require_matching_submodule_heads == b->require_matching_submodule_heads;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRepositoryDoctorPolicyTransferTails(UmiRepositoryDoctorPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRepositoryDoctorPolicyTransferMalformed(const UmiRepositoryDoctorPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRepositoryDoctorPolicyTransferCases, UmiRepositoryDoctorPolicy,
    umi_repository_doctor_policy_archive_encode, umi_repository_doctor_policy_archive_decode,
    UmiRepositoryDoctorPolicyTransferEqual, UmiRepositoryDoctorPolicyTransferTails, UmiRepositoryDoctorPolicyTransferMalformed)

int main(void){ UmiRepositoryDoctorPolicy p; umi_repository_doctor_policy_default(&p); assert(umi_repository_doctor_policy_validate(&p)==UMI_STATUS_OK);
    if (UmiRepositoryDoctorPolicyTransferCases(&p) != 0) return 1;
 assert(p.require_initialised_submodules); return 0; }
