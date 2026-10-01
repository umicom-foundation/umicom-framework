/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_notification_item.c
 * PURPOSE: Exercise ui notification_item snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/notification_item.h"
#include "umicom/ui/notification_item.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiNotificationItemSnapshot
#define CONTRACT_REGISTRY UmiUiNotificationItemRegistry
#define CONTRACT_CAPACITY UMI_UI_NOTIFICATION_ITEM_CAPACITY
#define CONTRACT_VALIDATE umi_ui_notification_item_snapshot_validate
#define CONTRACT_BATCH umi_ui_notification_item_registry_upsert_many
#define CONTRACT_CREATE umi_ui_notification_item_registry_create
#define CONTRACT_DESTROY umi_ui_notification_item_registry_destroy
#define CONTRACT_UPSERT umi_ui_notification_item_registry_upsert
#define CONTRACT_REMOVE umi_ui_notification_item_registry_remove
#define CONTRACT_FIND umi_ui_notification_item_registry_find
#define CONTRACT_AT umi_ui_notification_item_registry_at
#define CONTRACT_COUNT umi_ui_notification_item_registry_count
#define CONTRACT_REVISION umi_ui_notification_item_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiNotificationItemSnapshot, id), sizeof(((UmiUiNotificationItemSnapshot *)0)->id), 1 },
    {"title", offsetof(UmiUiNotificationItemSnapshot, title), sizeof(((UmiUiNotificationItemSnapshot *)0)->title), 0 },
    {"message", offsetof(UmiUiNotificationItemSnapshot, message), sizeof(((UmiUiNotificationItemSnapshot *)0)->message), 0 },
    {"source", offsetof(UmiUiNotificationItemSnapshot, source), sizeof(((UmiUiNotificationItemSnapshot *)0)->source), 0 },
    {"action_id", offsetof(UmiUiNotificationItemSnapshot, action_id), sizeof(((UmiUiNotificationItemSnapshot *)0)->action_id), 0 }
};
static int ContractSnapshotEqual(const UmiUiNotificationItemSnapshot *left,
    const UmiUiNotificationItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->message, right->message, sizeof(left->message)) == 0 &&
        memcmp(left->source, right->source, sizeof(left->source)) == 0 &&
        memcmp(left->action_id, right->action_id, sizeof(left->action_id)) == 0 &&
        left->timestamp == right->timestamp &&
        left->severity == right->severity &&
        left->read == right->read &&
        left->sticky == right->sticky &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiNotificationItemSnapshot *item)
{
    item->title[0] = 'v';
    item->message[0] = 'v';
    item->source[0] = 'v';
    item->action_id[0] = 'v';
    item->timestamp = (uint64_t)8U;
    item->severity = (int)9U;
    item->read = (int)10U;
    item->sticky = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_notification_item_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_notification_item_registry_replace_if_current
#include "snapshot_contract_cases.h"
