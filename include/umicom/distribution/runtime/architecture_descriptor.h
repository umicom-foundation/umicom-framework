/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/architecture_descriptor.h
 *
 * PURPOSE:
 *   CPU architecture, ABI, pointer-width and endianness descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_ARCHITECTURE_DESCRIPTOR_H
#define UMICOM_DISTRIBUTION_RUNTIME_ARCHITECTURE_DESCRIPTOR_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr architecture descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiDrArchitectureDescriptor { char id[UMI_DR_ID_CAPACITY]; UmiDrArchitecture architecture; UmiDrEndian endian; uint32_t pointer_bits; char abi[UMI_DR_ID_CAPACITY]; } UmiDrArchitectureDescriptor;
/**
 * Initialise dr architecture descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_architecture_descriptor_init(UmiDrArchitectureDescriptor *value);
/**
 * Check that dr architecture descriptor satisfies its contract before another service
 * relies on it.
 */
bool umi_dr_architecture_descriptor_valid(const UmiDrArchitectureDescriptor *value);
/**
 * Provide the dr architecture descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_architecture_descriptor_fingerprint(const UmiDrArchitectureDescriptor *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_architecture_descriptor_archive_encode(const UmiDrArchitectureDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_architecture_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiDrArchitectureDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
