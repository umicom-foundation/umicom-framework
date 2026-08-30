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

typedef struct UmiDrPackageProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrPackageFormat format; UmiDrInstallScope scope; uint32_t compression_level; bool include_symbols; bool deterministic; } UmiDrPackageProfile;
void umi_dr_package_profile_init(UmiDrPackageProfile *value);
bool umi_dr_package_profile_valid(const UmiDrPackageProfile *value);
uint64_t umi_dr_package_profile_fingerprint(const UmiDrPackageProfile *value);

#ifdef __cplusplus
}
#endif
#endif
