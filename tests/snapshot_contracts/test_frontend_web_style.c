/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_web_style.c
 * PURPOSE: Exercise frontend web_style snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/web_style.h"
#include "umicom/frontend/web_style.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendStyleSnapshot
#define CONTRACT_REGISTRY UmiFrontendStyleRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_WEB_STYLE_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_web_style_snapshot_validate
#define CONTRACT_BATCH umi_frontend_web_style_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_web_style_registry_create
#define CONTRACT_DESTROY umi_frontend_web_style_registry_destroy
#define CONTRACT_UPSERT umi_frontend_web_style_registry_upsert
#define CONTRACT_REMOVE umi_frontend_web_style_registry_remove
#define CONTRACT_FIND umi_frontend_web_style_registry_find
#define CONTRACT_AT umi_frontend_web_style_registry_at
#define CONTRACT_COUNT umi_frontend_web_style_registry_count
#define CONTRACT_REVISION umi_frontend_web_style_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendStyleSnapshot, id), sizeof(((UmiFrontendStyleSnapshot *)0)->id), 1 },
    {"selector", offsetof(UmiFrontendStyleSnapshot, selector), sizeof(((UmiFrontendStyleSnapshot *)0)->selector), 0 },
    {"property", offsetof(UmiFrontendStyleSnapshot, property), sizeof(((UmiFrontendStyleSnapshot *)0)->property), 0 },
    {"value", offsetof(UmiFrontendStyleSnapshot, value), sizeof(((UmiFrontendStyleSnapshot *)0)->value), 0 },
    {"media_query", offsetof(UmiFrontendStyleSnapshot, media_query), sizeof(((UmiFrontendStyleSnapshot *)0)->media_query), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendStyleSnapshot *left,
    const UmiFrontendStyleSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->selector, right->selector, sizeof(left->selector)) == 0 &&
        memcmp(left->property, right->property, sizeof(left->property)) == 0 &&
        memcmp(left->value, right->value, sizeof(left->value)) == 0 &&
        memcmp(left->media_query, right->media_query, sizeof(left->media_query)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendStyleSnapshot *item)
{
    item->selector[0] = 'v';
    item->property[0] = 'v';
    item->value[0] = 'v';
    item->media_query[0] = 'v';
    item->order = (int32_t)8U;
}
#include "snapshot_contract_cases.h"
