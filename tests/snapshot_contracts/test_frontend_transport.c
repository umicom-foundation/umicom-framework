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
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFrontendTransportEdit
#define CONTRACT_EDIT_CURRENT umi_frontend_transport_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_frontend_transport_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiFrontendTransportSnapshot ArchiveSample(void)
{
    UmiFrontendTransportSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiFrontendTransportSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->kind) + 1U;
        memset(value->kind + used, 0xa5, sizeof(value->kind) - used);
    }
    {
        size_t used = strlen(value->endpoint) + 1U;
        memset(value->endpoint + used, 0xa5, sizeof(value->endpoint) - used);
    }
}
#define ARCHIVE_TYPE UmiFrontendTransportSnapshot
#define ARCHIVE_ENCODE umi_frontend_transport_snapshot_archive_encode
#define ARCHIVE_DECODE umi_frontend_transport_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_frontend_transport_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_frontend_transport_registry_archive_restore
#include "snapshot_contract_cases.h"
