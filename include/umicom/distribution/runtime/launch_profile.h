/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/launch_profile.h
 *
 * PURPOSE:
 *   named launch profile with environment, frontend and safe-mode controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_LAUNCH_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_LAUNCH_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr launch profile data shared with callers of this public contract.
 */
typedef struct UmiDrLaunchProfile { char id[UMI_DR_ID_CAPACITY]; char launcher_id[UMI_DR_ID_CAPACITY]; char environment_id[UMI_DR_ID_CAPACITY]; char frontend[32]; bool safe_mode; bool offline; } UmiDrLaunchProfile;
/**
 * Initialise dr launch profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_launch_profile_init(UmiDrLaunchProfile *value);
/**
 * Check that dr launch profile satisfies its contract before another service relies on it.
 */
bool umi_dr_launch_profile_valid(const UmiDrLaunchProfile *value);
/**
 * Provide the dr launch profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_launch_profile_fingerprint(const UmiDrLaunchProfile *value);

#ifdef __cplusplus
}
#endif
#endif
