/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_abi.h
 *
 * PURPOSE:
 *   Parse and validate standard RISC-V ABI names against XLEN and floating-point capabilities.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_ABI_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_ABI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/abi_descriptor.h"
UmiStatus umi_ct_riscv_abi_descriptor(const char *abi,UmiCtAbiDescriptor *out_descriptor);
bool umi_ct_riscv_abi_matches_xlen(const UmiCtAbiDescriptor *descriptor,uint32_t xlen);

#ifdef __cplusplus
}
#endif

#endif
