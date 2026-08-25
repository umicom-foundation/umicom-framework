/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_isa.h
 *
 * PURPOSE:
 *   Parse RISC-V ISA strings into XLEN and stable capability masks without depending on compiler command lines.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_ISA_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_ISA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/cpu_feature_set.h"
typedef struct UmiCtRiscvIsa { uint32_t xlen; char canonical[128]; UmiCtCpuFeatureSet features; bool integer_base; bool multiply; bool zicsr; bool zifencei; } UmiCtRiscvIsa;
UmiStatus umi_ct_riscv_isa_parse(const char *isa,UmiCtRiscvIsa *out_isa);

#ifdef __cplusplus
}
#endif

#endif
