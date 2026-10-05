/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/lock_policy.h
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
#ifndef UMICOM_REPOSITORY_LOCK_POLICY_H
#define UMICOM_REPOSITORY_LOCK_POLICY_H
#include "umicom/repository/control_types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository lock policy data shared with callers of this public contract.
 */
typedef struct UmiRepositoryLockPolicy {
    int dry_run;
    int stage_gitlinks;
    int require_all_heads;
    int require_clean_parent;
    int verify_after_stage;
} UmiRepositoryLockPolicy;
/**
 * Initialise repository lock policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_lock_policy_init(UmiRepositoryLockPolicy *policy);
/**
 * Check that repository lock policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_lock_policy_validate(
    const UmiRepositoryLockPolicy *policy);
/**
 * Perform repository lock policy set dry through the module contract so client
 * applications do not duplicate its policy.
 */
void umi_repository_lock_policy_set_dry_run(
    UmiRepositoryLockPolicy *policy, int dry_run);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_repository_lock_policy_archive_encode(const UmiRepositoryLockPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_repository_lock_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiRepositoryLockPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
