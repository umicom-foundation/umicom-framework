/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_frontend_binding.c
 * PURPOSE: Exercise frontend binding snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/binding.h"
#include "umicom/frontend/binding.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiFrontendBindingSnapshot
#define CONTRACT_REGISTRY UmiFrontendBindingRegistry
#define CONTRACT_CAPACITY UMI_FRONTEND_BINDING_CAPACITY
#define CONTRACT_VALIDATE umi_frontend_binding_snapshot_validate
#define CONTRACT_BATCH umi_frontend_binding_registry_upsert_many
#define CONTRACT_CREATE umi_frontend_binding_registry_create
#define CONTRACT_DESTROY umi_frontend_binding_registry_destroy
#define CONTRACT_UPSERT umi_frontend_binding_registry_upsert
#define CONTRACT_REMOVE umi_frontend_binding_registry_remove
#define CONTRACT_FIND umi_frontend_binding_registry_find
#define CONTRACT_AT umi_frontend_binding_registry_at
#define CONTRACT_COUNT umi_frontend_binding_registry_count
#define CONTRACT_REVISION umi_frontend_binding_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiFrontendBindingSnapshot, id), sizeof(((UmiFrontendBindingSnapshot *)0)->id), 1 },
    {"source_path", offsetof(UmiFrontendBindingSnapshot, source_path), sizeof(((UmiFrontendBindingSnapshot *)0)->source_path), 0 },
    {"target_widget_id", offsetof(UmiFrontendBindingSnapshot, target_widget_id), sizeof(((UmiFrontendBindingSnapshot *)0)->target_widget_id), 0 },
    {"target_property", offsetof(UmiFrontendBindingSnapshot, target_property), sizeof(((UmiFrontendBindingSnapshot *)0)->target_property), 0 },
    {"converter", offsetof(UmiFrontendBindingSnapshot, converter), sizeof(((UmiFrontendBindingSnapshot *)0)->converter), 0 }
};
static int ContractSnapshotEqual(const UmiFrontendBindingSnapshot *left,
    const UmiFrontendBindingSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->source_path, right->source_path, sizeof(left->source_path)) == 0 &&
        memcmp(left->target_widget_id, right->target_widget_id, sizeof(left->target_widget_id)) == 0 &&
        memcmp(left->target_property, right->target_property, sizeof(left->target_property)) == 0 &&
        memcmp(left->converter, right->converter, sizeof(left->converter)) == 0 &&
        left->two_way == right->two_way &&
        left->enabled == right->enabled &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiFrontendBindingSnapshot *item)
{
    item->source_path[0] = 'v';
    item->target_widget_id[0] = 'v';
    item->target_property[0] = 'v';
    item->converter[0] = 'v';
    item->two_way = (int)8U;
    item->enabled = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_frontend_binding_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_frontend_binding_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiFrontendBindingEdit
#define CONTRACT_EDIT_CURRENT umi_frontend_binding_registry_edit_if_current
#include "snapshot_contract_cases.h"
