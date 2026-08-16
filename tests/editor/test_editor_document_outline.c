/*-----------------------------------------------------------------------------
 * Umicom Framework document outline tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/document_outline.h"

static UmiEditorIndexedSymbol outline_symbol(const char *id,
                                              const char *parent,
                                              const char *name,
                                              UmiEditorIndexedSymbolKind kind,
                                              uint64_t line,
                                              uint64_t end_line)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.parent_id, parent);
    (void)strcpy(value.provider_id, "outline-test");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, "main.c");
    (void)strcpy(value.language_id, "c");
    (void)strcpy(value.name, name);
    (void)strcpy(value.qualified_name, name);
    value.kind = kind;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///project/main.c",
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
    UmiEditorDocumentOutline *outline = NULL;
    UmiEditorIndexedSymbol function = outline_symbol("function", "", "main",
        UMI_EDITOR_INDEXED_SYMBOL_FUNCTION, 2U, 20U);
    UmiEditorIndexedSymbol local = outline_symbol("local", "function", "value",
        UMI_EDITOR_INDEXED_SYMBOL_VARIABLE, 5U, 5U);
    UmiEditorIndexedSymbol type = outline_symbol("type", "", "Options",
        UMI_EDITOR_INDEXED_SYMBOL_STRUCTURE, 25U, 30U);
    UmiEditorDocumentOutlineEntry entry;
    UmiEditorSourceLocation point;

    assert(umi_editor_symbol_index_create(0U, &index) == UMI_STATUS_OK);
    assert(umi_editor_document_outline_create(&outline) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &type) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &local) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &function) == UMI_STATUS_OK);
    assert(umi_editor_document_outline_build(outline, index, "main.c") ==
           UMI_STATUS_OK);
    assert(umi_editor_document_outline_count(outline) == 3U);
    assert(umi_editor_document_outline_entry_at(outline, 1U, &entry) ==
           UMI_STATUS_OK);
    assert(strcmp(entry.symbol_id, "local") == 0 && entry.depth == 1U);
    assert(umi_editor_document_outline_set_expanded(outline, "function", 0) ==
           UMI_STATUS_OK);
    assert(umi_editor_document_outline_visible_count(outline) == 2U);
    assert(umi_editor_source_location_initialize(&point,
                                                  "file:///project/main.c",
                                                  5U,
                                                  1U) == UMI_STATUS_OK);
    assert(umi_editor_document_outline_select_location(outline, &point) ==
           UMI_STATUS_OK);
    assert(umi_editor_document_outline_entry_at(outline, 1U, &entry) ==
           UMI_STATUS_OK && entry.selected);
    umi_editor_document_outline_destroy(outline);
    umi_editor_symbol_index_destroy(index);
    return 0;
}
