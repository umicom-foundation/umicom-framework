/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_symbol_index.c
 *
 * PURPOSE:
 *   Implement the test editor symbol index behavior for
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
 * Umicom Framework workspace symbol index tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/symbol_index.h"

static UmiEditorIndexedSymbol symbol(const char *id,
                                     const char *parent_id,
                                     const char *document_id,
                                     const char *name,
                                     UmiEditorIndexedSymbolKind kind,
                                     uint64_t line,
                                     uint64_t end_line)
{
    UmiEditorIndexedSymbol value = {0};
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    (void)strcpy(value.id, id);
    (void)strcpy(value.parent_id, parent_id);
    (void)strcpy(value.provider_id, "native-c-index");
    (void)strcpy(value.workspace_id, "workspace");
    (void)strcpy(value.document_id, document_id);
    (void)strcpy(value.language_id, "c");
    (void)strcpy(value.name, name);
    (void)strcpy(value.qualified_name, name);
    (void)strcpy(value.detail, "indexed C symbol");
    value.kind = kind;
    value.flags = UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION;
    assert(umi_editor_source_location_initialize(&value.location,
                                                  "file:///project/main.c",
                                                  line,
                                                  0U) == UMI_STATUS_OK);
    value.location.end_line = end_line;
    value.location.end_column = 80U;
    value.selection_location = value.location;
    value.selection_location.end_line = line;
    value.selection_location.end_column = 8U;
    return value;
}

int main(void)
{
    UmiEditorSymbolIndex *index = NULL;
    UmiEditorIndexedSymbol root = symbol("main", "", "main.c", "main",
        UMI_EDITOR_INDEXED_SYMBOL_FUNCTION, 3U, 20U);
    UmiEditorIndexedSymbol local = symbol("main.value", "main", "main.c",
        "value", UMI_EDITOR_INDEXED_SYMBOL_VARIABLE, 6U, 6U);
    UmiEditorIndexedSymbol helper = symbol("helper", "", "helper.c",
        "format_value", UMI_EDITOR_INDEXED_SYMBOL_FUNCTION, 30U, 40U);
    UmiEditorIndexedSymbol found;
    UmiEditorSourceLocation point;
    UmiEditorIndexedSymbol children[4];
    UmiEditorSymbolMatch matches[4];
    UmiEditorSymbolQuery query = {0};
    size_t count = 0U;
    size_t removed = 0U;

    assert(umi_editor_symbol_index_create(2U, &index) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &root) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &local) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_upsert(index, &helper) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_count(index) == 3U);
    assert(umi_editor_symbol_index_find(index, "main.value", &found) ==
           UMI_STATUS_OK);
    assert(strcmp(found.name, "value") == 0);

    assert(umi_editor_source_location_initialize(&point,
                                                  "file:///project/main.c",
                                                  6U,
                                                  2U) == UMI_STATUS_OK);
    assert(umi_editor_symbol_index_find_enclosing(index, &point, &found) ==
           UMI_STATUS_OK);
    assert(strcmp(found.id, "main.value") == 0);
    assert(umi_editor_symbol_index_children(index, "main", "main.c",
                                            children, 4U, &count) ==
           UMI_STATUS_OK);
    assert(count == 1U && strcmp(children[0].id, "main.value") == 0);

    query.struct_size = (uint32_t)sizeof(query);
    query.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
    query.text = "fmtval";
    query.maximum_results = 4U;
    query.include_external = 1;
    assert(umi_editor_symbol_index_search(index, &query, matches, 4U, &count) ==
           UMI_STATUS_OK);
    assert(count == 1U && strcmp(matches[0].symbol.id, "helper") == 0);
    assert(matches[0].score > 0);
    assert(umi_editor_symbol_index_remove_document(index, "main.c", &removed) ==
           UMI_STATUS_OK);
    assert(removed == 2U && umi_editor_symbol_index_count(index) == 1U);
    assert(strcmp(umi_editor_indexed_symbol_kind_name(
                      UMI_EDITOR_INDEXED_SYMBOL_FUNCTION), "function") == 0);
    assert(umi_editor_indexed_symbol_kind_from_name("structure") ==
           UMI_EDITOR_INDEXED_SYMBOL_STRUCTURE);
    umi_editor_symbol_index_destroy(index);
    return 0;
}
