/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_designer_property_schema.c
 * PURPOSE: Check designer property_schema input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/property_schema.h"
#include "umicom/designer/property_schema.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDesignerPropertySchemaSnapshot
#define CONTRACT_REGISTRY UmiDesignerPropertySchemaRegistry
#define CONTRACT_CAPACITY UMI_DESIGNER_PROPERTY_SCHEMA_CAPACITY
#define CONTRACT_VALIDATE umi_designer_property_schema_snapshot_validate
#define CONTRACT_BATCH umi_designer_property_schema_registry_upsert_many
#define CONTRACT_CREATE umi_designer_property_schema_registry_create
#define CONTRACT_DESTROY umi_designer_property_schema_registry_destroy
#define CONTRACT_UPSERT umi_designer_property_schema_registry_upsert
#define CONTRACT_REMOVE umi_designer_property_schema_registry_remove
#define CONTRACT_FIND umi_designer_property_schema_registry_find
#define CONTRACT_AT umi_designer_property_schema_registry_at
#define CONTRACT_COUNT umi_designer_property_schema_registry_count
#define CONTRACT_REVISION umi_designer_property_schema_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDesignerPropertySchemaSnapshot, id), sizeof(((UmiDesignerPropertySchemaSnapshot *)0)->id), 1 },
    {"component_type", offsetof(UmiDesignerPropertySchemaSnapshot, component_type), sizeof(((UmiDesignerPropertySchemaSnapshot *)0)->component_type), 0 },
    {"property_name", offsetof(UmiDesignerPropertySchemaSnapshot, property_name), sizeof(((UmiDesignerPropertySchemaSnapshot *)0)->property_name), 0 },
    {"value_type", offsetof(UmiDesignerPropertySchemaSnapshot, value_type), sizeof(((UmiDesignerPropertySchemaSnapshot *)0)->value_type), 0 },
    {"default_value", offsetof(UmiDesignerPropertySchemaSnapshot, default_value), sizeof(((UmiDesignerPropertySchemaSnapshot *)0)->default_value), 0 },
    {"category", offsetof(UmiDesignerPropertySchemaSnapshot, category), sizeof(((UmiDesignerPropertySchemaSnapshot *)0)->category), 0 }
};
static int ContractSnapshotEqual(const UmiDesignerPropertySchemaSnapshot *left, const UmiDesignerPropertySchemaSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->component_type, right->component_type, sizeof(left->component_type)) == 0 &&
        memcmp(left->property_name, right->property_name, sizeof(left->property_name)) == 0 &&
        memcmp(left->value_type, right->value_type, sizeof(left->value_type)) == 0 &&
        memcmp(left->default_value, right->default_value, sizeof(left->default_value)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        left->required == right->required &&
        left->bindable == right->bindable &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiDesignerPropertySchemaSnapshot *item)
{
    item->component_type[0] = 'v';
    item->property_name[0] = 'v';
    item->value_type[0] = 'v';
    item->default_value[0] = 'v';
    item->category[0] = 'v';
    item->required = (int)9U;
    item->bindable = (int)10U;
    item->order = (int32_t)11U;
}
/* Exercise public capture and publication through this domain's real owner,
 * including its field normalisation and failure-without-mutation contract. */
#define CONTRACT_CAPTURE umi_designer_property_schema_registry_capture
#define CONTRACT_REPLACE_CURRENT umi_designer_property_schema_registry_replace_if_current
/* Exercise mixed edits through this domain's public types and owner. */
#define CONTRACT_EDIT UmiDesignerPropertySchemaEdit
#define CONTRACT_EDIT_CURRENT umi_designer_property_schema_registry_edit_if_current
/* Page checks use this domain's real owner and complete payload comparator. */
#define CONTRACT_READ_PAGE umi_designer_property_schema_registry_read_page
/* Build a complete public snapshot; registry normalization is checked
 * separately by the collection restore cases. */
static UmiDesignerPropertySchemaSnapshot ArchiveSample(void)
{
    UmiDesignerPropertySchemaSnapshot value = {0};
    ContractPayload(&value);
    memcpy(value.id, "archive-record", sizeof("archive-record"));
    value.struct_size = (uint32_t)sizeof(value);
    value.revision = 17U;
    return value;
}
/* Unused tails are not part of a C string and must not enter saved bytes. */
static void ArchiveFillUnusedText(UmiDesignerPropertySchemaSnapshot *value)
{
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->component_type) + 1U;
        memset(value->component_type + used, 0xa5, sizeof(value->component_type) - used);
    }
    {
        size_t used = strlen(value->property_name) + 1U;
        memset(value->property_name + used, 0xa5, sizeof(value->property_name) - used);
    }
    {
        size_t used = strlen(value->value_type) + 1U;
        memset(value->value_type + used, 0xa5, sizeof(value->value_type) - used);
    }
    {
        size_t used = strlen(value->default_value) + 1U;
        memset(value->default_value + used, 0xa5, sizeof(value->default_value) - used);
    }
    {
        size_t used = strlen(value->category) + 1U;
        memset(value->category + used, 0xa5, sizeof(value->category) - used);
    }
}
#define ARCHIVE_TYPE UmiDesignerPropertySchemaSnapshot
#define ARCHIVE_ENCODE umi_designer_property_schema_snapshot_archive_encode
#define ARCHIVE_DECODE umi_designer_property_schema_snapshot_archive_decode
#define ARCHIVE_EQUAL ContractSnapshotEqual
#include "../value_archive/record_cases.h"

#define CONTRACT_ARCHIVE_ENCODE umi_designer_property_schema_registry_archive_encode
#define CONTRACT_ARCHIVE_RESTORE umi_designer_property_schema_registry_archive_restore
#include "snapshot_contract_cases.h"
