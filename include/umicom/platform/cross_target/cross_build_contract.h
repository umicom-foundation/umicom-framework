/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/cross_build_contract.h
 *
 * PURPOSE:
 *   Declare cross-build requirements while leaving actual compiler/tool discovery to the existing Toolchain subsystem.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_CROSS_BUILD_CONTRACT_H
#define UMICOM_PLATFORM_CROSS_TARGET_CROSS_BUILD_CONTRACT_H

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
 * Represent the ct cross build contract data shared with callers of this public contract.
 */
typedef struct UmiCtCrossBuildContract { char contract_id[UMI_CT_ID_CAPACITY]; UmiCtTarget target; char required_toolchain_family[32]; char required_abi[32]; bool require_sysroot; bool require_emulator; bool require_debugger; bool require_assembly; } UmiCtCrossBuildContract;
/**
 * Check that ct cross build contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ct_cross_build_contract_validate(const UmiCtCrossBuildContract *contract);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_cross_build_contract_archive_encode(const UmiCtCrossBuildContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_cross_build_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiCtCrossBuildContract *value);

#ifdef __cplusplus
}
#endif

#endif
