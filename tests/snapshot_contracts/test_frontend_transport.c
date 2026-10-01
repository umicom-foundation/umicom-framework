/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_transport.c
 * PURPOSE: Exercise frontend transport snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/transport.h"
#include "umicom/frontend/transport.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendTransportSnapshot
#define CONTRACT_REGISTRY UmiFrontendTransportRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_TRANSPORT_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_transport_snapshot_validate
#define CONTRACT_BATCH umi_frontend_transport_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_transport_registry_create
#define CONTRACT_DESTROY umi_frontend_transport_registry_destroy
#define CONTRACT_UPSERT umi_frontend_transport_registry_upsert
#define CONTRACT_REMOVE umi_frontend_transport_registry_remove
#define CONTRACT_FIND umi_frontend_transport_registry_find
#define CONTRACT_AT umi_frontend_transport_registry_at
#define CONTRACT_COUNT umi_frontend_transport_registry_count
#define CONTRACT_REVISION umi_frontend_transport_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendTransportSnapshot, id), sizeof(((UmiFrontendTransportSnapshot *)0)->id), 1 },
    {"session_id", offsetof(UmiFrontendTransportSnapshot, session_id), sizeof(((UmiFrontendTransportSnapshot *)0)->session_id), 0 },
    {"kind", offsetof(UmiFrontendTransportSnapshot, kind), sizeof(((UmiFrontendTransportSnapshot *)0)->kind), 0 },
    {"endpoint", offsetof(UmiFrontendTransportSnapshot, endpoint), sizeof(((UmiFrontendTransportSnapshot *)0)->endpoint), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendTransportSnapshot *left,
    const UmiFrontendTransportSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->session_id, right->session_id, sizeof(left->session_id)) == 0 &&
        memcmp(left->kind, right->kind, sizeof(left->kind)) == 0 &&
        memcmp(left->endpoint, right->endpoint, sizeof(left->endpoint)) == 0 &&
        left->sent_messages == right->sent_messages &&
        left->received_messages == right->received_messages &&
        left->connected == right->connected &&
        left->fallback_allowed == right->fallback_allowed &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendTransportSnapshot *item)
{
    item->session_id[0] = 'v';
    item->kind[0] = 'v';
    item->endpoint[0] = 'v';
    item->sent_messages = (uint64_t)7U;
    item->received_messages = (uint64_t)8U;
    item->connected = (int)9U;
    item->fallback_allowed = (int)10U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_transport_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_transport_registry_replace_if_current
#include "snapshot_contract_cases.h"
