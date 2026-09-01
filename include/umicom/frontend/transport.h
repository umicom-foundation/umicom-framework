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

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_TRANSPORT_CAPACITY 1024U

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

typedef struct UmiFrontendTransportRegistry UmiFrontendTransportRegistry;

UmiStatus umi_frontend_transport_registry_create(UmiFrontendTransportRegistry **out_registry);
void umi_frontend_transport_registry_destroy(UmiFrontendTransportRegistry *registry);
UmiStatus umi_frontend_transport_registry_upsert(UmiFrontendTransportRegistry *registry, const UmiFrontendTransportSnapshot *item);
UmiStatus umi_frontend_transport_registry_remove(UmiFrontendTransportRegistry *registry, const char *id);
UmiStatus umi_frontend_transport_registry_find(const UmiFrontendTransportRegistry *registry, const char *id, UmiFrontendTransportSnapshot *out_item);
UmiStatus umi_frontend_transport_registry_at(const UmiFrontendTransportRegistry *registry, size_t index, UmiFrontendTransportSnapshot *out_item);
size_t umi_frontend_transport_registry_count(const UmiFrontendTransportRegistry *registry);
uint64_t umi_frontend_transport_registry_revision(const UmiFrontendTransportRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
