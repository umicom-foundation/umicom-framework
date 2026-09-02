/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_vector_profile.h
 *
 * PURPOSE:
 *   Model RISC-V Vector minimum VLEN/ELEN requirements for accelerated Framework kernels.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_VECTOR_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_VECTOR_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ct riscv vector profile data shared with callers of this public contract.
 */
typedef struct UmiCtRiscvVectorProfile { uint32_t minimum_vlen; uint32_t minimum_elen; bool required; } UmiCtRiscvVectorProfile;
/**
 * Provide the ct riscv vector profile satisfied operation used by this module and its
 * client applications.
 */
bool umi_ct_riscv_vector_profile_satisfied(const UmiCtRiscvVectorProfile *profile,uint32_t vlen,uint32_t elen,bool vector_present);

#ifdef __cplusplus
}
#endif

#endif
