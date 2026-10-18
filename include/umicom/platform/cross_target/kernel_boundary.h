/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/kernel_boundary.h
 *
 * PURPOSE:
 *   Define explicit user/kernel/hypervisor ownership boundaries for Umicom OS reusable services.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_KERNEL_BOUNDARY_H
#define UMICOM_PLATFORM_CROSS_TARGET_KERNEL_BOUNDARY_H

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
 * List the named ct boundary domain values accepted by this public contract.
 */
typedef enum UmiCtBoundaryDomain { UMI_CT_DOMAIN_USER=1, UMI_CT_DOMAIN_KERNEL=2, UMI_CT_DOMAIN_HYPERVISOR=3, UMI_CT_DOMAIN_FIRMWARE=4 } UmiCtBoundaryDomain;
/**
 * Represent the ct kernel boundary data shared with callers of this public contract.
 */
typedef struct UmiCtKernelBoundary { char boundary_id[UMI_CT_ID_CAPACITY]; UmiCtBoundaryDomain caller; UmiCtBoundaryDomain callee; bool copy_in; bool copy_out; bool privileged; } UmiCtKernelBoundary;
/**
 * Check that ct kernel boundary satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ct_kernel_boundary_validate(const UmiCtKernelBoundary *boundary);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_kernel_boundary_archive_encode(const UmiCtKernelBoundary *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_kernel_boundary_archive_decode(const void *bytes, size_t byte_count,
    UmiCtKernelBoundary *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCtKernelBoundary object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ct_kernel_boundary_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCtKernelBoundary *workspace, UmiCtKernelBoundary *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ct_kernel_boundary_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCtKernelBoundary *expected, UmiCtKernelBoundary *value);


#ifdef __cplusplus
}
#endif

#endif
