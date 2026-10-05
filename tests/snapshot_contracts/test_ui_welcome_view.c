/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_ui_welcome_view.c
 * PURPOSE: Exercise ui welcome_view snapshot boundaries and batch rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/welcome_view.h"
#include "umicom/ui/welcome_view.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiUiWelcomeItemSnapshot
#define CONTRACT_REGISTRY UmiUiWelcomeItemRegistry
#define CONTRACT_CAPACITY UMI_UI_WELCOME_VIEW_CAPACITY
#define CONTRACT_VALIDATE umi_ui_welcome_view_snapshot_validate
#define CONTRACT_BATCH umi_ui_welcome_view_registry_upsert_many
#define CONTRACT_CREATE umi_ui_welcome_view_registry_create
#define CONTRACT_DESTROY umi_ui_welcome_view_registry_destroy
#define CONTRACT_UPSERT umi_ui_welcome_view_registry_upsert
#define CONTRACT_REMOVE umi_ui_welcome_view_registry_remove
#define CONTRACT_FIND umi_ui_welcome_view_registry_find
#define CONTRACT_AT umi_ui_welcome_view_registry_at
#define CONTRACT_COUNT umi_ui_welcome_view_registry_count
#define CONTRACT_REVISION umi_ui_welcome_view_registry_revision

/* Explicit test expectations refer to public member boundaries, including
 * every optional text field. Non-text values are compared by value, not padding. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiUiWelcomeItemSnapshot, id), sizeof(((UmiUiWelcomeItemSnapshot *)0)->id), 1 },
    {"view_id", offsetof(UmiUiWelcomeItemSnapshot, view_id), sizeof(((UmiUiWelcomeItemSnapshot *)0)->view_id), 0 },
    {"title", offsetof(UmiUiWelcomeItemSnapshot, title), sizeof(((UmiUiWelcomeItemSnapshot *)0)->title), 0 },
    {"description", offsetof(UmiUiWelcomeItemSnapshot, description), sizeof(((UmiUiWelcomeItemSnapshot *)0)->description), 0 },
    {"command_id", offsetof(UmiUiWelcomeItemSnapshot, command_id), sizeof(((UmiUiWelcomeItemSnapshot *)0)->command_id), 0 },
    {"when_expression", offsetof(UmiUiWelcomeItemSnapshot, when_expression), sizeof(((UmiUiWelcomeItemSnapshot *)0)->when_expression), 0 }
};
static int ContractSnapshotEqual(const UmiUiWelcomeItemSnapshot *left,
    const UmiUiWelcomeItemSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->view_id, right->view_id, sizeof(left->view_id)) == 0 &&
        memcmp(left->title, right->title, sizeof(left->title)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        memcmp(left->command_id, right->command_id, sizeof(left->command_id)) == 0 &&
        memcmp(left->when_expression, right->when_expression, sizeof(left->when_expression)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose accidentally dropped scalar or optional fields. */
static void ContractPayload(UmiUiWelcomeItemSnapshot *item)
{
    item->view_id[0] = 'v';
    item->title[0] = 'v';
    item->description[0] = 'v';
    item->command_id[0] = 'v';
    item->when_expression[0] = 'v';
    item->order = (int32_t)9U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_ui_welcome_view_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_ui_welcome_view_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiUiWelcomeItemEdit
#define CONTRACT_EDIT_CURRENT umi_ui_welcome_view_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_ui_welcome_view_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiUiWelcomeItemSnapshot ArchiveSample(void)
{
    UmiUiWelcomeItemSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiUiWelcomeItemSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->view_id) + 1U;
        memset(value->view_id + used, 0xa5, sizeof(value->view_id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
    {
        size_t used = strlen(value->command_id) + 1U;
        memset(value->command_id + used, 0xa5, sizeof(value->command_id) - used);
    }
    {
        size_t used = strlen(value->when_expression) + 1U;
        memset(value->when_expression + used, 0xa5, sizeof(value->when_expression) - used);
    }
}
#define ARCHIVE_TYPE UmiUiWelcomeItemSnapshot
#define ARCHIVE_ENCODE umi_ui_welcome_view_snapshot_archive_encode
#define ARCHIVE_DECODE umi_ui_welcome_view_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_ui_welcome_view_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_ui_welcome_view_registry_archive_restore
#include "snapshot_contract_cases.h"
