/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/endpoint_descriptor.h
 *
 * PURPOSE:
 *   Describe a protocol endpoint independently from any transport implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ENDPOINT_DESCRIPTOR_H
#define UMICOM_INTEGRATION_FABRIC_ENDPOINT_DESCRIPTOR_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric endpoint descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricEndpointDescriptor {
    char endpoint_id[UMI_FABRIC_ID_CAPACITY];
    char uri[UMI_FABRIC_URI_CAPACITY];
    UmiFabricProtocol protocol;
    bool secure;
    uint32_t weight;
} UmiFabricEndpointDescriptor;

/**
 * Initialise fabric endpoint descriptor from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_fabric_endpoint_descriptor_init(UmiFabricEndpointDescriptor *item, const char *endpoint_id, const char *uri, UmiFabricProtocol protocol, bool secure, uint32_t weight);
/**
 * Check that fabric endpoint descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_endpoint_descriptor_validate(const UmiFabricEndpointDescriptor *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_endpoint_descriptor_archive_encode(const UmiFabricEndpointDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_endpoint_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricEndpointDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fabric_endpoint_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFabricEndpointDescriptor *expected, UmiFabricEndpointDescriptor *value);


#ifdef __cplusplus
}
#endif
#endif
