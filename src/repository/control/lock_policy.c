/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/lock_policy.c
 *
 * PURPOSE:
 *   Define safe native submodule-lock policy including dry-run semantics.
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
#include "umicom/repository/lock_policy.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise repository lock policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_lock_policy_init(UmiRepositoryLockPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->stage_gitlinks = 1;
    policy->require_all_heads = 1;
    policy->verify_after_stage = 1;
}

/*
 * Perform repository lock policy set dry through the module contract so client
 * applications do not duplicate its policy.
 */
void umi_repository_lock_policy_set_dry_run(
    UmiRepositoryLockPolicy *policy, int dry_run)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    policy->dry_run = dry_run != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->dry_run) policy->stage_gitlinks = 0;
}

/*
 * Check that repository lock policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_lock_policy_validate(
    const UmiRepositoryLockPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->dry_run && policy->stage_gitlinks) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRepositoryLockPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x81fea5bc9830c60e);

    return schema;
}
static size_t UmiRepositoryLockPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRepositoryLockPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiRepositoryLockPolicy *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->dry_run);
    UmiArchiveWriteSigned(writer, (int64_t)value->stage_gitlinks);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_all_heads);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_clean_parent);
    UmiArchiveWriteSigned(writer, (int64_t)value->verify_after_stage);
}
static void UmiRepositoryLockPolicyArchiveRead(UmiArchiveReader *reader, UmiRepositoryLockPolicy *value)
{
    value->dry_run = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->stage_gitlinks = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_all_heads = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_clean_parent = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->verify_after_stage = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiRepositoryLockPolicyArchiveValidate(const UmiRepositoryLockPolicy *value)
{
    return umi_repository_lock_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_repository_lock_policy_archive_encode, umi_repository_lock_policy_archive_decode,
    UmiRepositoryLockPolicy, UmiRepositoryLockPolicyArchiveSchema, UmiRepositoryLockPolicyArchiveBound, UmiRepositoryLockPolicyArchiveWrite, UmiRepositoryLockPolicyArchiveRead, UmiRepositoryLockPolicyArchiveValidate)
