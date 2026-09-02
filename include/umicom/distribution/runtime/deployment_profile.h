/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/deployment_profile.h
 *
 * PURPOSE:
 *   deployment target, scope, rollout and update-channel profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DEPLOYMENT_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_DEPLOYMENT_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr deployment profile data shared with callers of this public contract.
 */
typedef struct UmiDrDeploymentProfile { char id[UMI_DR_ID_CAPACITY]; char target[UMI_DR_TEXT_CAPACITY]; UmiDrInstallScope scope; UmiDrChannelKind channel; uint32_t rollout_percent; bool unattended; } UmiDrDeploymentProfile;
/**
 * Initialise dr deployment profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_deployment_profile_init(UmiDrDeploymentProfile *value);
/**
 * Check that dr deployment profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_deployment_profile_valid(const UmiDrDeploymentProfile *value);
/**
 * Provide the dr deployment profile fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_deployment_profile_fingerprint(const UmiDrDeploymentProfile *value);

#ifdef __cplusplus
}
#endif
#endif
