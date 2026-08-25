/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/abi_compatibility.h
 *
 * PURPOSE:
 *   Classify ABI compatibility and identify the first material incompatibility.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_ABI_COMPATIBILITY_H
#define UMICOM_PLATFORM_CROSS_TARGET_ABI_COMPATIBILITY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/abi_descriptor.h"
typedef enum UmiCtAbiCompatibility { UMI_CT_ABI_INCOMPATIBLE=0, UMI_CT_ABI_BRIDGE_REQUIRED=1, UMI_CT_ABI_COMPATIBLE=2 } UmiCtAbiCompatibility;
UmiCtAbiCompatibility umi_ct_abi_compare(const UmiCtAbiDescriptor *left,const UmiCtAbiDescriptor *right,char *reason,size_t reason_capacity);

#ifdef __cplusplus
}
#endif

#endif
