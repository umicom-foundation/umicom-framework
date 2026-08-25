/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_profile.h
 *
 * PURPOSE:
 *   named runtime profiles combining platform, architecture and capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDrRuntimeProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrArchitecture architecture; UmiDrVersion minimum_version; uint64_t required_capabilities; uint64_t score_bias; } UmiDrRuntimeProfile;
void umi_dr_runtime_profile_init(UmiDrRuntimeProfile *value);
bool umi_dr_runtime_profile_valid(const UmiDrRuntimeProfile *value);
uint64_t umi_dr_runtime_profile_fingerprint(const UmiDrRuntimeProfile *value);

#ifdef __cplusplus
}
#endif
#endif
