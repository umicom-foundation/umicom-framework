/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/transport.h
 *
 * PURPOSE:
 *   Define frontend transport state for WebSocket, event-stream and request/response delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_FRONTEND_TRANSPORT_H
#define UMICOM_FRONTEND_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_TRANSPORT_CAPACITY 1024U

/**
 * Represent the frontend transport snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendTransportSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char kind[64];
    char endpoint[512];
    uint64_t sent_messages;
    uint64_t received_messages;
    int connected;
    int fallback_allowed;
    uint64_t revision;
} UmiFrontendTransportSnapshot;

/**
 * Represent the frontend transport registry data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendTransportRegistry UmiFrontendTransportRegistry;

/**
 * Initialise frontend transport registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_transport_registry_create(UmiFrontendTransportRegistry **out_registry);
/**
 * Release or reset state held by frontend transport registry so the same storage can be
 * reused safely.
 */
void umi_frontend_transport_registry_destroy(UmiFrontendTransportRegistry *registry);
/**
 * Provide the frontend transport registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_transport_registry_upsert(UmiFrontendTransportRegistry *registry, const UmiFrontendTransportSnapshot *item);
/**
 * Remove frontend transport registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_transport_registry_remove(UmiFrontendTransportRegistry *registry, const char *id);
/**
 * Find frontend transport registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_transport_registry_find(const UmiFrontendTransportRegistry *registry, const char *id, UmiFrontendTransportSnapshot *out_item);
/**
 * Find frontend transport registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_transport_registry_at(const UmiFrontendTransportRegistry *registry, size_t index, UmiFrontendTransportSnapshot *out_item);
/**
 * Return the number of records represented by frontend transport registry without changing
 * their state.
 */
size_t umi_frontend_transport_registry_count(const UmiFrontendTransportRegistry *registry);
/**
 * Provide the frontend transport registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_transport_registry_revision(const UmiFrontendTransportRegistry *registry);


/** Check id and every fixed text array before lookup/copy. id must be nonempty;
 * other text may be empty. Output is optional and contains field diagnostics,
 * not input text. No state changes or allocations occur. This validates text
 * bounds, not domain semantics or UTF-8. Size/version normalisation is unchanged.
 * The existing registry_upsert operation applies the same check before mutation. */
UmiStatus umi_frontend_transport_snapshot_validate(const UmiFrontendTransportSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Atomically insert/replace up to UMI_FRONTEND_TRANSPORT_CAPACITY distinct IDs in memory.
 * Entries retain normal upsert order and revision increments. Duplicate IDs
 * within the batch return ALREADY_EXISTS; IDs already stored may be replaced.
 * Failure publishes no changes. An empty batch succeeds without allocation.
 * Inputs remain caller-owned and unchanged. outResult is optional and must not
 * overlap inputs or registry storage. Keep them stable and serialize registry
 * access on the owning thread. This copies a full registry temporarily and
 * performs no I/O; it is not a filesystem or cross-thread transaction. */
UmiStatus umi_frontend_transport_registry_upsert_many(UmiFrontendTransportRegistry *registry,
    const UmiFrontendTransportSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
