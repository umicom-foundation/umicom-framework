/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/operation_guard.c
 *
 * PURPOSE:
 *   Implement preconditions that protect mutating source-control operations.
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
#include "umicom/vcs/advanced/operation_guard.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced operation guard from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_operation_guard_init(UmiVcsAdvancedOperationGuard *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->require_no_conflicts = 1;
    value->safety = UMI_VCS_SAFETY_REVIEW;
}

/*
 * Check that vcs advanced operation guard satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_operation_guard_validate(const UmiVcsAdvancedOperationGuard *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->safety > UMI_VCS_SAFETY_DESTRUCTIVE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced operation guard allows operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_operation_guard_allows(const UmiVcsAdvancedOperationGuard *guard,
                                               int worktree_clean,
                                               int conflicts,
                                               int has_upstream,
                                               int unpushed_commits,
                                               int detached_head)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_operation_guard_validate(guard) != UMI_STATUS_OK) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_clean_worktree && !worktree_clean) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_no_conflicts && conflicts) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_upstream && !has_upstream) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_no_unpushed_commits && unpushed_commits) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!guard->allow_detached_head && detached_head) return 0;
    return 1;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedOperationGuardArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x255e0f745f99e09e);

    return schema;
}
static size_t UmiVcsAdvancedOperationGuardArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedOperationGuardArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedOperationGuard *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_clean_worktree);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_no_conflicts);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_upstream);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_no_unpushed_commits);
    UmiArchiveWriteSigned(writer, (int64_t)value->allow_detached_head);
    UmiArchiveWriteSigned(writer, (int64_t)value->safety);
}
static void UmiVcsAdvancedOperationGuardArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedOperationGuard *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->require_clean_worktree = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_no_conflicts = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_upstream = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_no_unpushed_commits = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->allow_detached_head = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->safety = (UmiVcsSafetyLevel)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiVcsAdvancedOperationGuardArchiveValidate(const UmiVcsAdvancedOperationGuard *value)
{
    return umi_vcs_advanced_operation_guard_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_operation_guard_archive_encode, umi_vcs_advanced_operation_guard_archive_decode,
    UmiVcsAdvancedOperationGuard, UmiVcsAdvancedOperationGuardArchiveSchema, UmiVcsAdvancedOperationGuardArchiveBound, UmiVcsAdvancedOperationGuardArchiveWrite, UmiVcsAdvancedOperationGuardArchiveRead, UmiVcsAdvancedOperationGuardArchiveValidate)
