/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/riscv_platform.h
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_RISCV_PLATFORM_H
#define UMICOM_PLATFORM_CROSS_TARGET_RISCV_PLATFORM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ct riscv machine values accepted by this public contract.
 */
typedef enum UmiCtRiscvMachine { UMI_CT_RISCV_MACHINE_GENERIC=1, UMI_CT_RISCV_MACHINE_QEMU_VIRT=2, UMI_CT_RISCV_MACHINE_SIFIVE_U=3, UMI_CT_RISCV_MACHINE_UMICOM=4 } UmiCtRiscvMachine;
/**
 * Represent the ct riscv platform data shared with callers of this public contract.
 */
typedef struct UmiCtRiscvPlatform { char platform_id[UMI_CT_ID_CAPACITY]; UmiCtRiscvMachine machine; uint64_t memory_bytes; uint32_t cpu_count; bool plic; bool clint; bool pci; bool virtio; } UmiCtRiscvPlatform;
/**
 * Check that ct riscv platform satisfies its contract before another service relies on it.
 */
UmiStatus umi_ct_riscv_platform_validate(const UmiCtRiscvPlatform *platform);

#ifdef __cplusplus
}
#endif

#endif
