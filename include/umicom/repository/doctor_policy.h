/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/doctor_policy.h
 *
 * PURPOSE:
 *   Define reusable repository doctor acceptance policy without mutating Git state.
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
#ifndef INCLUDE_UMICOM_REPOSITORY_DOCTOR_POLICY_H
#define INCLUDE_UMICOM_REPOSITORY_DOCTOR_POLICY_H
#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository doctor policy data shared with callers of this public contract.
 */
typedef struct UmiRepositoryDoctorPolicy {
    int allow_dirty_worktree;
    int require_origin;
    int require_upstream;
    int require_initialised_submodules;
    int require_matching_submodule_heads;
} UmiRepositoryDoctorPolicy;

/**
 * Provide the repository doctor policy default operation used by this module and its
 * client applications.
 */
void umi_repository_doctor_policy_default(UmiRepositoryDoctorPolicy *policy);
/**
 * Check that repository doctor policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_doctor_policy_validate(const UmiRepositoryDoctorPolicy *policy);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_repository_doctor_policy_archive_encode(const UmiRepositoryDoctorPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_repository_doctor_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiRepositoryDoctorPolicy *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_repository_doctor_policy_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRepositoryDoctorPolicy *expected, UmiRepositoryDoctorPolicy *value);


#ifdef __cplusplus
}
#endif
#endif
