/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_platform.c
 *
 * PURPOSE:
 *   Describe RISC-V board/virtual-machine platform capabilities used by boot and device planning.
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

#include "umicom/platform/cross_target/riscv_platform.h"

UmiStatus umi_ct_riscv_platform_validate(const UmiCtRiscvPlatform*p){if(p==NULL||!umi_ct_id_valid(p->platform_id)||p->cpu_count==0U||p->memory_bytes<UINT64_C(16)*1024U*1024U)return UMI_STATUS_INVALID_ARGUMENT;if(p->machine==UMI_CT_RISCV_MACHINE_QEMU_VIRT&&(!p->plic||!p->clint))return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
