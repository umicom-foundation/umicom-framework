/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_profile.h
 *
 * PURPOSE:
 *   Define named RISC-V deployment profiles for Linux, bare-metal and Umicom OS targets.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_PROFILE_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/riscv_isa.h"
/**
 * Represent the ct riscv profile data shared with callers of this public contract.
 */
typedef struct UmiCtRiscvProfile { char profile_id[UMI_CT_ID_CAPACITY]; UmiCtRiscvIsa minimum_isa; uint32_t minimum_cores; bool requires_mmu; bool supervisor_mode; } UmiCtRiscvProfile;
/**
 * Provide the ct riscv profile builtin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ct_riscv_profile_builtin(const char *profile_id,UmiCtRiscvProfile *out_profile);
/**
 * Provide the ct riscv profile satisfied operation used by this module and its client
 * applications.
 */
bool umi_ct_riscv_profile_satisfied(const UmiCtRiscvProfile *profile,const UmiCtRiscvIsa *available,uint32_t cores,bool mmu);

#ifdef __cplusplus
}
#endif

#endif
