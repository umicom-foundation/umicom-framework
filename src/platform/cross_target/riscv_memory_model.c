/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_memory_model.c
 *
 * PURPOSE:
 *   Describe RISC-V MMU modes, address widths and page sizes used by kernel/runtime portability checks.
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

#include "umicom/platform/cross_target/riscv_memory_model.h"

UmiStatus umi_ct_riscv_memory_model_validate(const UmiCtRiscvMemoryModel*m,uint32_t x){if(m==NULL||(x!=32U&&x!=64U)||m->page_size<4096U||(m->page_size&(m->page_size-1U))!=0U)return UMI_STATUS_INVALID_ARGUMENT;if(x==32U&&m->mmu>UMI_CT_RISCV_MMU_SV32)return UMI_STATUS_INVALID_STATE;if(x==64U&&m->mmu==UMI_CT_RISCV_MMU_SV32)return UMI_STATUS_INVALID_STATE;if(m->virtual_address_bits>x)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
