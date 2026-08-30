/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/target_profile.h
 *
 * PURPOSE:
 *   Define named deployment/runtime profiles that bind a target identity to ABI and minimum CPU capability requirements.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TARGET_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_TARGET_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCtTargetProfile { char profile_id[UMI_CT_ID_CAPACITY]; UmiCtTarget target; char abi[32]; uint64_t required_cpu_features; uint32_t minimum_page_size; bool production_supported; } UmiCtTargetProfile;
UmiStatus umi_ct_target_profile_init(UmiCtTargetProfile *profile,const char *id,const UmiCtTarget *target,const char *abi);
bool umi_ct_target_profile_satisfied(const UmiCtTargetProfile *profile,uint64_t cpu_features,uint32_t page_size);

#ifdef __cplusplus
}
#endif

#endif
