/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/target_profile.h
 *
 * PURPOSE:
 *   Define native code-generation profiles for x86-64 and RISC-V without replacing canonical compiler triples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TARGET_PROFILE_H
#define UMICOM_COMPILER_NATIVE_TARGET_PROFILE_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native target profile data shared with callers of this public contract.
 */
typedef struct UmiNativeTargetProfile { UmiNativeMachineArch architecture; UmiNativeObjectFormat object_format; char triple[UMI_NC_NAME_CAPACITY]; uint32_t pointer_bits; bool little_endian; uint64_t feature_mask; } UmiNativeTargetProfile;
/**
 * Initialise nc target profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_target_profile_init(UmiNativeTargetProfile *profile,const char *triple);
/**
 * Provide the nc target profile compatible operation used by this module and its client
 * applications.
 */
bool umi_nc_target_profile_compatible(const UmiNativeTargetProfile *a,const UmiNativeTargetProfile *b);
#ifdef __cplusplus
}
#endif
#endif
