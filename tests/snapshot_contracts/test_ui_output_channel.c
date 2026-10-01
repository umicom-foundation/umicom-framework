/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_output_channel.c
 * PURPOSE: Exercise ui output_channel snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/output_channel.h"
#include "umicom/ui/output_channel.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiOutputChannelSnapshot
#define CONTRACT_REGISTRY UmiUiOutputChannelRegistry
#define CONTRACT_CAPACITY UMI_UI_OUTPUT_CHANNEL_CAPACITY
#define CONTRACT_VALIDATE umi_ui_output_channel_snapshot_validate
#define CONTRACT_BATCH umi_ui_output_channel_registry_upsert_many
#define CONTRACT_CREATE umi_ui_output_channel_registry_create
#define CONTRACT_DESTROY umi_ui_output_channel_registry_destroy
#define CONTRACT_UPSERT umi_ui_output_channel_registry_upsert
#define CONTRACT_REMOVE umi_ui_output_channel_registry_remove
#define CONTRACT_FIND umi_ui_output_channel_registry_find
#define CONTRACT_AT umi_ui_output_channel_registry_at
#define CONTRACT_COUNT umi_ui_output_channel_registry_count
#define CONTRACT_REVISION umi_ui_output_channel_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiOutputChannelSnapshot, id), sizeof(((UmiUiOutputChannelSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiUiOutputChannelSnapshot, name), sizeof(((UmiUiOutputChannelSnapshot *)0)->name), 0 },
    {"category", offsetof(UmiUiOutputChannelSnapshot, category), sizeof(((UmiUiOutputChannelSnapshot *)0)->category), 0 },
    {"text", offsetof(UmiUiOutputChannelSnapshot, text), sizeof(((UmiUiOutputChannelSnapshot *)0)->text), 0 }
};
static int ContractSnapshotEqual(const UmiUiOutputChannelSnapshot *left,
    const UmiUiOutputChannelSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->text, right->text, sizeof(left->text)) == 0 &&
        left->sequence == right->sequence &&
        left->visible == right->visible &&
        left->preserve == right->preserve &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiOutputChannelSnapshot *item)
{
    item->name[0] = 'v';
    item->category[0] = 'v';
    item->text[0] = 'v';
    item->sequence = (uint64_t)7U;
    item->visible = (int)8U;
    item->preserve = (int)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_output_channel_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_output_channel_registry_replace_if_current
#include "snapshot_contract_cases.h"
