/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_import_organisation.c
 *
 * PURPOSE:
 *   Implement the test editor import organisation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_import_organisation.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/import_organisation.h"

static UmiEditorImportItem import_item(const char *id,
                                       const char *source,
                                       UmiEditorImportKind kind,
                                       int used,
                                       int pinned)
{
    UmiEditorImportItem item = {0};

    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = UMI_EDITOR_IMPORT_ORGANISATION_API_VERSION;
    (void)strcpy(item.id, id);
    (void)strcpy(item.provider_id, "native-c");
    (void)strcpy(item.canonical_source, source);
    (void)strcpy(item.original_text, "#include <stdio.h>");
    assert(umi_editor_source_location_initialize(
               &item.location, "file:///workspace/main.c", 0U, 0U) ==
           UMI_STATUS_OK);
    item.location.end_byte_offset = 18U;
    item.location.end_column = 18U;
    item.kind = kind;
    item.group_order = (int32_t)kind;
    item.used = used;
    item.pinned = pinned;
    return item;
}

int main(void)
{
    UmiEditorImportOrganisationPlan *plan = NULL;
    UmiEditorImportItem stdio = import_item(
        "stdio", "stdio.h", UMI_EDITOR_IMPORT_SYSTEM, 1, 0);
    UmiEditorImportItem duplicate = import_item(
        "stdio-copy", "stdio.h", UMI_EDITOR_IMPORT_SYSTEM, 1, 0);
    UmiEditorImportItem unused = import_item(
        "unused", "unused.h", UMI_EDITOR_IMPORT_PROJECT, 0, 0);
    UmiEditorImportItem kept;
    UmiEditorImportOrganisationSnapshot snapshot;

    assert(umi_editor_import_organisation_plan_create(&plan) == UMI_STATUS_OK);
    assert(umi_editor_import_organisation_plan_upsert(plan, &unused) ==
           UMI_STATUS_OK);
    assert(umi_editor_import_organisation_plan_upsert(plan, &duplicate) ==
           UMI_STATUS_OK);
    assert(umi_editor_import_organisation_plan_upsert(plan, &stdio) ==
           UMI_STATUS_OK);
    assert(umi_editor_import_organisation_plan_analyse(plan) == UMI_STATUS_OK);
    assert(umi_editor_import_organisation_plan_sort(plan) == UMI_STATUS_OK);
    assert(umi_editor_import_organisation_plan_snapshot(plan, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.import_count == 3U);
    assert(snapshot.duplicate_count == 1U);
    assert(snapshot.unused_count == 1U);
    assert(snapshot.removable_count == 2U);
    assert(umi_editor_import_organisation_plan_kept_at(plan, 0U, &kept) ==
           UMI_STATUS_OK);
    assert(strcmp(kept.canonical_source, "stdio.h") == 0);
    umi_editor_import_organisation_plan_destroy(plan);
    return 0;
}
