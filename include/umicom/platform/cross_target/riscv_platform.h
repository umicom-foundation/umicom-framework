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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_riscv_platform_archive_encode(const UmiCtRiscvPlatform *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_riscv_platform_archive_decode(const void *bytes, size_t byte_count,
    UmiCtRiscvPlatform *value);

#ifdef __cplusplus
}
#endif

#endif
