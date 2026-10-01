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
#include "snapshot_contract_cases.h"
