/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/abi_descriptor.h
 *
 * PURPOSE:
 *   Describe data model, calling convention, stack alignment and floating-point ABI properties for cross-target compatibility checks.
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
#ifndef UMICOM_PLATFORM_CROSS_TARGET_ABI_DESCRIPTOR_H
#define UMICOM_PLATFORM_CROSS_TARGET_ABI_DESCRIPTOR_H

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
 * List the named ct data model values accepted by this public contract.
 */
typedef enum UmiCtDataModel { UMI_CT_DATA_ILP32=1, UMI_CT_DATA_LP64=2, UMI_CT_DATA_LLP64=3 } UmiCtDataModel;
/**
 * List the named ct calling convention values accepted by this public contract.
 */
typedef enum UmiCtCallingConvention { UMI_CT_CALL_C=1, UMI_CT_CALL_SYSV=2, UMI_CT_CALL_WIN64=3, UMI_CT_CALL_RISCV=4, UMI_CT_CALL_UMICOM=5 } UmiCtCallingConvention;
/**
 * Represent the ct abi descriptor data shared with callers of this public contract.
 */
typedef struct UmiCtAbiDescriptor { char abi_id[32]; UmiCtDataModel data_model; UmiCtCallingConvention calling_convention; uint32_t pointer_bits; uint32_t stack_alignment; uint32_t long_double_bits; bool hard_float; } UmiCtAbiDescriptor;
/**
 * Check that ct abi descriptor satisfies its contract before another service relies on it.
 */
UmiStatus umi_ct_abi_descriptor_validate(const UmiCtAbiDescriptor *descriptor);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_abi_descriptor_archive_encode(const UmiCtAbiDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_abi_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiCtAbiDescriptor *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiCtAbiDescriptor object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ct_abi_descriptor_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiCtAbiDescriptor *workspace, UmiCtAbiDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ct_abi_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCtAbiDescriptor *expected, UmiCtAbiDescriptor *value);


#ifdef __cplusplus
}
#endif

#endif
