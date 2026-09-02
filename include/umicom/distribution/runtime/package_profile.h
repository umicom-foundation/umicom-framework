/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/package_profile.h
 *
 * PURPOSE:
 *   named package profile selecting format, scope, compression and symbols policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr package profile data shared with callers of this public contract.
 */
typedef struct UmiDrPackageProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrPackageFormat format; UmiDrInstallScope scope; uint32_t compression_level; bool include_symbols; bool deterministic; } UmiDrPackageProfile;
/**
 * Initialise dr package profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_package_profile_init(UmiDrPackageProfile *value);
/**
 * Check that dr package profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_package_profile_valid(const UmiDrPackageProfile *value);
/**
 * Provide the dr package profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_package_profile_fingerprint(const UmiDrPackageProfile *value);

#ifdef __cplusplus
}
#endif
#endif
