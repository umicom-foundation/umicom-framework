/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_web_session.c
 * PURPOSE: Exercise frontend web_session snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/web_session.h"
#include "umicom/frontend/web_session.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendSessionSnapshot
#define CONTRACT_REGISTRY UmiFrontendSessionRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_WEB_SESSION_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_web_session_snapshot_validate
#define CONTRACT_BATCH umi_frontend_web_session_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_web_session_registry_create
#define CONTRACT_DESTROY umi_frontend_web_session_registry_destroy
#define CONTRACT_UPSERT umi_frontend_web_session_registry_upsert
#define CONTRACT_REMOVE umi_frontend_web_session_registry_remove
#define CONTRACT_FIND umi_frontend_web_session_registry_find
#define CONTRACT_AT umi_frontend_web_session_registry_at
#define CONTRACT_COUNT umi_frontend_web_session_registry_count
#define CONTRACT_REVISION umi_frontend_web_session_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendSessionSnapshot, id), sizeof(((UmiFrontendSessionSnapshot *)0)->id), 1 },
    {"user_id", offsetof(UmiFrontendSessionSnapshot, user_id), sizeof(((UmiFrontendSessionSnapshot *)0)->user_id), 0 },
    {"route", offsetof(UmiFrontendSessionSnapshot, route), sizeof(((UmiFrontendSessionSnapshot *)0)->route), 0 },
    {"transport", offsetof(UmiFrontendSessionSnapshot, transport), sizeof(((UmiFrontendSessionSnapshot *)0)->transport), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendSessionSnapshot *left,
    const UmiFrontendSessionSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->user_id, right->user_id, sizeof(left->user_id)) == 0 &&
        memcmp(left->route, right->route, sizeof(left->route)) == 0 &&
        memcmp(left->transport, right->transport, sizeof(left->transport)) == 0 &&
        left->created_at == right->created_at &&
        left->last_activity == right->last_activity &&
        left->authenticated == right->authenticated &&
        left->connected == right->connected &&
        left->suspended == right->suspended &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendSessionSnapshot *item)
{
    item->user_id[0] = 'v';
    item->route[0] = 'v';
    item->transport[0] = 'v';
    item->created_at = (uint64_t)7U;
    item->last_activity = (uint64_t)8U;
    item->authenticated = (int)9U;
    item->connected = (int)10U;
    item->suspended = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_web_session_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_web_session_registry_replace_if_current
#include "snapshot_contract_cases.h"
