/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_extension.h
 *
 * PURPOSE:
 *   Map standard RISC-V ISA extension names onto Framework CPU capability bits.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_EXTENSION_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_EXTENSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/cpu_feature.h"
/**
 * Provide the ct riscv extension feature operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_riscv_extension_feature(const char *extension,UmiCtCpuFeature *out_feature);
/**
 * Provide the ct riscv extension known operation used by this module and its client
 * applications.
 */
bool umi_ct_riscv_extension_known(const char *extension);

#ifdef __cplusplus
}
#endif

#endif
