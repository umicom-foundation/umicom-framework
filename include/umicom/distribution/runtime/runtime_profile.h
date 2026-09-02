/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_profile.h
 *
 * PURPOSE:
 *   named runtime profiles combining platform, architecture and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr runtime profile data shared with callers of this public contract.
 */
typedef struct UmiDrRuntimeProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrArchitecture architecture; UmiDrVersion minimum_version; uint64_t required_capabilities; uint64_t score_bias; } UmiDrRuntimeProfile;
/**
 * Initialise dr runtime profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_runtime_profile_init(UmiDrRuntimeProfile *value);
/**
 * Check that dr runtime profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_runtime_profile_valid(const UmiDrRuntimeProfile *value);
/**
 * Provide the dr runtime profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_runtime_profile_fingerprint(const UmiDrRuntimeProfile *value);

#ifdef __cplusplus
}
#endif
#endif
