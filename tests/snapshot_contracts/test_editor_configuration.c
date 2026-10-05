/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_editor_configuration.c
 * PURPOSE: Check editor configuration input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/configuration.h"
#include "umicom/editor/configuration.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiEditorConfigurationSnapshot
#define CONTRACT_REGISTRY UmiEditorConfigurationRegistry
#define CONTRACT_CAPACITY UMI_EDITOR_CONFIGURATION_CAPACITY
#define CONTRACT_VALIDATE umi_editor_configuration_snapshot_validate
#define CONTRACT_BATCH umi_editor_configuration_registry_upsert_many
#define CONTRACT_CREATE umi_editor_configuration_registry_create
#define CONTRACT_DESTROY umi_editor_configuration_registry_destroy
#define CONTRACT_UPSERT umi_editor_configuration_registry_upsert
#define CONTRACT_REMOVE umi_editor_configuration_registry_remove
#define CONTRACT_FIND umi_editor_configuration_registry_find
#define CONTRACT_AT umi_editor_configuration_registry_at
#define CONTRACT_COUNT umi_editor_configuration_registry_count
#define CONTRACT_REVISION umi_editor_configuration_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiEditorConfigurationSnapshot, id), sizeof(((UmiEditorConfigurationSnapshot *)0)->id), 1 },
    {"language_id", offsetof(UmiEditorConfigurationSnapshot, language_id), sizeof(((UmiEditorConfigurationSnapshot *)0)->language_id), 0 }
};
static int ContractSnapshotEqual(const UmiEditorConfigurationSnapshot *left, const UmiEditorConfigurationSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->language_id, right->language_id, sizeof(left->language_id)) == 0 &&
        left->tab_size == right->tab_size &&
        left->insert_spaces == right->insert_spaces &&
        left->word_wrap == right->word_wrap &&
        left->line_numbers == right->line_numbers &&
        left->minimap == right->minimap &&
        left->auto_indent == right->auto_indent &&
        left->format_on_save == right->format_on_save &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiEditorConfigurationSnapshot *item)
{
    item->language_id[0] = 'v';
    item->tab_size = (uint32_t)5U;
    item->insert_spaces = (int)6U;
    item->word_wrap = (int)7U;
    item->line_numbers = (int)8U;
    item->minimap = (int)9U;
    item->auto_indent = (int)10U;
    item->format_on_save = (int)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_editor_configuration_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_editor_configuration_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiEditorConfigurationEdit
#define CONTRACT_EDIT_CURRENT umi_editor_configuration_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_editor_configuration_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiEditorConfigurationSnapshot ArchiveSample(void)
{
    UmiEditorConfigurationSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiEditorConfigurationSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->language_id) + 1U;
        memset(value->language_id + used, 0xa5, sizeof(value->language_id) - used);
    }
}
#define ARCHIVE_TYPE UmiEditorConfigurationSnapshot
#define ARCHIVE_ENCODE umi_editor_configuration_snapshot_archive_encode
#define ARCHIVE_DECODE umi_editor_configuration_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_editor_configuration_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_editor_configuration_registry_archive_restore
#include "snapshot_contract_cases.h"
