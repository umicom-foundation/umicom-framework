/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/snapshot_contracts/test_designer_template_palette.c
 * PURPOSE: Check designer template_palette input bounds and atomic imports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/template_palette.h"
#include "umicom/designer/template_palette.h"
#include <stddef.h>
#include <string.h>

#define CONTRACT_SNAPSHOT UmiDesignerTemplatePaletteSnapshot
#define CONTRACT_REGISTRY UmiDesignerTemplatePaletteRegistry
#define CONTRACT_CAPACITY UMI_DESIGNER_TEMPLATE_PALETTE_CAPACITY
#define CONTRACT_VALIDATE umi_designer_template_palette_snapshot_validate
#define CONTRACT_BATCH umi_designer_template_palette_registry_upsert_many
#define CONTRACT_CREATE umi_designer_template_palette_registry_create
#define CONTRACT_DESTROY umi_designer_template_palette_registry_destroy
#define CONTRACT_UPSERT umi_designer_template_palette_registry_upsert
#define CONTRACT_REMOVE umi_designer_template_palette_registry_remove
#define CONTRACT_FIND umi_designer_template_palette_registry_find
#define CONTRACT_AT umi_designer_template_palette_registry_at
#define CONTRACT_COUNT umi_designer_template_palette_registry_count
#define CONTRACT_REVISION umi_designer_template_palette_registry_revision

/* Test each actual public field boundary, including optional provider text. */
static const UmiSnapshotTextField contract_fields[] = {
    {"id", offsetof(UmiDesignerTemplatePaletteSnapshot, id), sizeof(((UmiDesignerTemplatePaletteSnapshot *)0)->id), 1 },
    {"name", offsetof(UmiDesignerTemplatePaletteSnapshot, name), sizeof(((UmiDesignerTemplatePaletteSnapshot *)0)->name), 0 },
    {"category", offsetof(UmiDesignerTemplatePaletteSnapshot, category), sizeof(((UmiDesignerTemplatePaletteSnapshot *)0)->category), 0 },
    {"description", offsetof(UmiDesignerTemplatePaletteSnapshot, description), sizeof(((UmiDesignerTemplatePaletteSnapshot *)0)->description), 0 },
    {"template_id", offsetof(UmiDesignerTemplatePaletteSnapshot, template_id), sizeof(((UmiDesignerTemplatePaletteSnapshot *)0)->template_id), 0 },
    {"preview_uri", offsetof(UmiDesignerTemplatePaletteSnapshot, preview_uri), sizeof(((UmiDesignerTemplatePaletteSnapshot *)0)->preview_uri), 0 }
};
static int ContractSnapshotEqual(const UmiDesignerTemplatePaletteSnapshot *left, const UmiDesignerTemplatePaletteSnapshot *right)
{
    return left->struct_size == right->struct_size &&
        left->api_version == right->api_version &&
        memcmp(left->id, right->id, sizeof(left->id)) == 0 &&
        memcmp(left->name, right->name, sizeof(left->name)) == 0 &&
        memcmp(left->category, right->category, sizeof(left->category)) == 0 &&
        memcmp(left->description, right->description, sizeof(left->description)) == 0 &&
        memcmp(left->template_id, right->template_id, sizeof(left->template_id)) == 0 &&
        memcmp(left->preview_uri, right->preview_uri, sizeof(left->preview_uri)) == 0 &&
        left->order == right->order &&
        left->revision == right->revision;
}
/* Nonzero payloads expose lost optional text and numeric fields. */
static void ContractPayload(UmiDesignerTemplatePaletteSnapshot *item)
{
    item->name[0] = 'v';
    item->category[0] = 'v';
    item->description[0] = 'v';
    item->template_id[0] = 'v';
    item->preview_uri[0] = 'v';
    item->order = (int32_t)9U;
}
#include "snapshot_contract_cases.h"
