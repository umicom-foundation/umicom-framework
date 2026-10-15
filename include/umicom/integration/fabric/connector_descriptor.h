/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/connector_descriptor.h
 *
 * PURPOSE:
 *   Describe a connector implementation and supported protocol/capability surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CONNECTOR_DESCRIPTOR_H
#define UMICOM_INTEGRATION_FABRIC_CONNECTOR_DESCRIPTOR_H

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
 * Represent the fabric connector descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricConnectorDescriptor {
    char connector_id[UMI_FABRIC_ID_CAPACITY];
    char provider[UMI_FABRIC_TEXT_CAPACITY];
    UmiFabricProtocol protocol;
    uint64_t capability_mask;
    bool supports_transactions;
} UmiFabricConnectorDescriptor;

/**
 * Initialise fabric connector descriptor from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_fabric_connector_descriptor_init(UmiFabricConnectorDescriptor *item, const char *connector_id, const char *provider, UmiFabricProtocol protocol, uint64_t capability_mask, bool supports_transactions);
/**
 * Check that fabric connector descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_connector_descriptor_validate(const UmiFabricConnectorDescriptor *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_connector_descriptor_archive_encode(const UmiFabricConnectorDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_connector_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricConnectorDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fabric_connector_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFabricConnectorDescriptor *expected, UmiFabricConnectorDescriptor *value);


#ifdef __cplusplus
}
#endif
#endif
