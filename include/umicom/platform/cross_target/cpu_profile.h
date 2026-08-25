/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cpu_profile.h
 *
 * PURPOSE:
 *   Describe deployable CPU profiles and evaluate runtime feature/XLEN compatibility.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CPU_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_CPU_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/cpu_feature_set.h"
typedef struct UmiCtCpuProfile { char profile_id[UMI_CT_ID_CAPACITY]; UmiCtArchitecture architecture; uint32_t xlen; UmiCtCpuFeatureSet required; uint32_t minimum_cores; } UmiCtCpuProfile;
UmiStatus umi_ct_cpu_profile_validate(const UmiCtCpuProfile *profile);
bool umi_ct_cpu_profile_matches(const UmiCtCpuProfile *profile,UmiCtArchitecture architecture,uint32_t xlen,uint32_t cores,const UmiCtCpuFeatureSet *available);

#ifdef __cplusplus
}
#endif

#endif
