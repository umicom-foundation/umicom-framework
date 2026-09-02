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

#ifdef __cplusplus
}
#endif
#endif
