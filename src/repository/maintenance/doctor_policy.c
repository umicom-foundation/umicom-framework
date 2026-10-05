/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/doctor_policy.c
 *
 * PURPOSE:
 *   Implement conservative default repository doctor policy.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/doctor_policy.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Provide the repository doctor policy default operation used by this module and its
 * client applications.
 */
void umi_repository_doctor_policy_default(UmiRepositoryDoctorPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->require_initialised_submodules = 1;
    policy->require_matching_submodule_heads = 1;
}

/*
 * Check that repository doctor policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_doctor_policy_validate(const UmiRepositoryDoctorPolicy *policy)
{
    return policy != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiRepositoryDoctorPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcdef7027f85eddef);

    return schema;
}
static size_t UmiRepositoryDoctorPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRepositoryDoctorPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiRepositoryDoctorPolicy *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->allow_dirty_worktree);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_origin);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_upstream);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_initialised_submodules);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_matching_submodule_heads);
}
static void UmiRepositoryDoctorPolicyArchiveRead(UmiArchiveReader *reader, UmiRepositoryDoctorPolicy *value)
{
    value->allow_dirty_worktree = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_origin = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_upstream = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_initialised_submodules = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_matching_submodule_heads = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiRepositoryDoctorPolicyArchiveValidate(const UmiRepositoryDoctorPolicy *value)
{
    return umi_repository_doctor_policy_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_repository_doctor_policy_archive_encode, umi_repository_doctor_policy_archive_decode,
    UmiRepositoryDoctorPolicy, UmiRepositoryDoctorPolicyArchiveSchema, UmiRepositoryDoctorPolicyArchiveBound, UmiRepositoryDoctorPolicyArchiveWrite, UmiRepositoryDoctorPolicyArchiveRead, UmiRepositoryDoctorPolicyArchiveValidate)
