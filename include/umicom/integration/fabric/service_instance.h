/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_instance.h
 *
 * PURPOSE:
 *   Represent a live service instance advertised to the Fabric registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_INSTANCE_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_INSTANCE_H

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
 * Represent the fabric service instance data shared with callers of this public contract.
 */
typedef struct UmiFabricServiceInstance {
    char instance_id[UMI_FABRIC_ID_CAPACITY];
    char service_id[UMI_FABRIC_ID_CAPACITY];
    char endpoint_id[UMI_FABRIC_ID_CAPACITY];
    uint32_t priority;
    uint32_t weight;
    bool healthy;
    uint64_t last_seen_ms;
} UmiFabricServiceInstance;

/**
 * Initialise fabric service instance from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_fabric_service_instance_init(UmiFabricServiceInstance *item, const char *instance_id, const char *service_id, const char *endpoint_id, uint32_t priority, uint32_t weight);
/**
 * Check that fabric service instance satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_service_instance_validate(const UmiFabricServiceInstance *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_service_instance_archive_encode(const UmiFabricServiceInstance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_service_instance_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricServiceInstance *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fabric_service_instance_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFabricServiceInstance *expected, UmiFabricServiceInstance *value);


#ifdef __cplusplus
}
#endif
#endif
