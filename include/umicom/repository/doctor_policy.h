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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_DOCTOR_POLICY_H
#define INCLUDE_UMICOM_REPOSITORY_DOCTOR_POLICY_H
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryDoctorPolicy {
    int allow_dirty_worktree;
    int require_origin;
    int require_upstream;
    int require_initialised_submodules;
    int require_matching_submodule_heads;
} UmiRepositoryDoctorPolicy;

void umi_repository_doctor_policy_default(UmiRepositoryDoctorPolicy *policy);
UmiStatus umi_repository_doctor_policy_validate(const UmiRepositoryDoctorPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
