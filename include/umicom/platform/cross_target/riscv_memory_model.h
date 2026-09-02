/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_memory_model.h
 *
 * PURPOSE:
 *   Describe RISC-V MMU modes, address widths and page sizes used by kernel/runtime portability checks.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_MEMORY_MODEL_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_MEMORY_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ct riscv mmu values accepted by this public contract.
 */
typedef enum UmiCtRiscvMmu { UMI_CT_RISCV_MMU_NONE=0, UMI_CT_RISCV_MMU_SV32=1, UMI_CT_RISCV_MMU_SV39=2, UMI_CT_RISCV_MMU_SV48=3, UMI_CT_RISCV_MMU_SV57=4 } UmiCtRiscvMmu;
/**
 * Represent the ct riscv memory model data shared with callers of this public contract.
 */
typedef struct UmiCtRiscvMemoryModel { UmiCtRiscvMmu mmu; uint32_t physical_address_bits; uint32_t virtual_address_bits; uint32_t page_size; bool executable_pages; } UmiCtRiscvMemoryModel;
/**
 * Check that ct riscv memory model satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_riscv_memory_model_validate(const UmiCtRiscvMemoryModel *model,uint32_t xlen);

#ifdef __cplusplus
}
#endif

#endif
