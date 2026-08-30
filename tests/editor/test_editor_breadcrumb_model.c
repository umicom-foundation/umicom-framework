/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_breadcrumb_model.c
 *
 * PURPOSE:
 *   Implement the test editor breadcrumb model behavior for
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
 * Umicom Framework breadcrumb model tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/breadcrumb_model.h"

static UmiEditorIndexedSymbol breadcrumb_symbol(const char *id,
                                                 const char *parent,
                                                 const char *name,
                                                 uint64_t line,
                                                 uint64_t end_line)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.parent_id, parent);
    (void)strcpy(value.provider_id, "breadcrumb-test");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, "main.c");
    (void)strcpy(value.language_id, "c");
    (void)strcpy(value.name, name);
    (void)strcpy(value.qualified_name, name);
    value.kind = UMI_EDITOR_INDEXED_SYMBOL_FUNCTION;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///project/src/main.c",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.end_line = end_line;
    value.location.end_column = 80U;
    value.selection_location = value.location;
    return value;
}

int main(void)
{
    UmiEditorSymbolIndex *index = NULL;
    UmiEditorBreadcrumbModel *model = NULL;
    UmiEditorIndexedSymbol outer = breadcrumb_symbol("outer", "", "outer",
                                                      2U, 20U);
    UmiEditorIndexedSymbol inner = breadcrumb_symbol("inner", "outer", "inner",
                                                      5U, 10U);
    UmiEditorSourceLocation point;
    UmiEditorBreadcrumbSegment segment;

    assert(umi_editor_symbol_index_create(0U, &index) == UMI_STATUS_OK);
    assert(umi_editor_breadcrumb_model_create(&model) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &outer) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &inner) == UMI_STATUS_OK);
    assert(umi_editor_source_location_initialize(&point,
                                                  "file:///project/src/main.c",
                                                  6U,
                                                  2U) == UMI_STATUS_OK);
    assert(umi_editor_breadcrumb_model_build(model, index, "Workspace", &point) ==
           UMI_STATUS_OK);
    assert(umi_editor_breadcrumb_model_count(model) == 6U);
    assert(umi_editor_breadcrumb_model_at(model, 0U, &segment) == UMI_STATUS_OK);
    assert(segment.kind == UMI_EDITOR_BREADCRUMB_WORKSPACE);
    assert(umi_editor_breadcrumb_model_at(model, 3U, &segment) == UMI_STATUS_OK);
    assert(segment.kind == UMI_EDITOR_BREADCRUMB_FILE);
    assert(strcmp(segment.label, "main.c") == 0);
    assert(umi_editor_breadcrumb_model_at(model, 5U, &segment) == UMI_STATUS_OK);
    assert(segment.kind == UMI_EDITOR_BREADCRUMB_SYMBOL);
    assert(strcmp(segment.label, "inner") == 0 && segment.active);
    assert(umi_editor_breadcrumb_model_set_active(model, 4U) == UMI_STATUS_OK);
    assert(umi_editor_breadcrumb_model_active_index(model) == 4U);
    umi_editor_breadcrumb_model_destroy(model);
    umi_editor_symbol_index_destroy(index);
    return 0;
}
