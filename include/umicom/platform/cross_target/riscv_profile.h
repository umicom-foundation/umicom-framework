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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiCtRiscvProfile { char profile_id[UMI_CT_ID_CAPACITY]; UmiCtRiscvIsa minimum_isa; uint32_t minimum_cores; bool requires_mmu; bool supervisor_mode; } UmiCtRiscvProfile;
UmiStatus umi_ct_riscv_profile_builtin(const char *profile_id,UmiCtRiscvProfile *out_profile);
bool umi_ct_riscv_profile_satisfied(const UmiCtRiscvProfile *profile,const UmiCtRiscvIsa *available,uint32_t cores,bool mmu);

#ifdef __cplusplus
}
#endif

#endif
