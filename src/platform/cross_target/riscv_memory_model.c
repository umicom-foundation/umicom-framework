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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/riscv_memory_model.h"

/*
 * Check that ct riscv memory model satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_riscv_memory_model_validate(const UmiCtRiscvMemoryModel*m,uint32_t x){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||(x!=32U&&x!=64U)||m->page_size<4096U||(m->page_size&(m->page_size-1U))!=0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x==32U&&m->mmu>UMI_CT_RISCV_MMU_SV32)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(x==64U&&m->mmu==UMI_CT_RISCV_MMU_SV32)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->virtual_address_bits>x)return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
