/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cpu_feature.h
 *
 * PURPOSE:
 *   Define stable generic and RISC-V CPU feature bits used by runtime capability negotiation.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CPU_FEATURE_H
#define UMICOM_PLATFORM_CROSS_TARGET_CPU_FEATURE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ct cpu feature values accepted by this public contract.
 */
typedef enum UmiCtCpuFeature { UMI_CT_CPU_ATOMICS=0, UMI_CT_CPU_FLOAT=1, UMI_CT_CPU_DOUBLE=2, UMI_CT_CPU_COMPRESSED=3, UMI_CT_CPU_VECTOR=4, UMI_CT_CPU_BITMANIP=5, UMI_CT_CPU_CRYPTO=6, UMI_CT_CPU_HYPERVISOR=7, UMI_CT_CPU_USER_INTERRUPTS=8, UMI_CT_CPU_MMU=9, UMI_CT_CPU_COUNT=10 } UmiCtCpuFeature;
/**
 * Provide the ct cpu feature bit operation used by this module and its client
 * applications.
 */
uint64_t umi_ct_cpu_feature_bit(UmiCtCpuFeature feature);
/**
 * Provide the ct cpu feature text operation used by this module and its client
 * applications.
 */
const char *umi_ct_cpu_feature_text(UmiCtCpuFeature feature);

#ifdef __cplusplus
}
#endif

#endif
