/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/document_outline.h
 *
 * PURPOSE:
 *   Build a hierarchical, collapsible and selectable document outline from
 *   the canonical workspace symbol index for any desktop or web frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_DOCUMENT_OUTLINE_H
#define UMICOM_EDITOR_DOCUMENT_OUTLINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/symbol_index.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_DOCUMENT_OUTLINE_API_VERSION 1U

typedef struct UmiEditorDocumentOutlineEntry {
    uint32_t struct_size;
    uint32_t api_version;
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char parent_symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char label[UMI_EDITOR_SYMBOL_NAME_CAPACITY];
    char detail[UMI_EDITOR_SYMBOL_DETAIL_CAPACITY];
    UmiEditorIndexedSymbolKind kind;
    UmiEditorSourceLocation location;
    UmiEditorSourceLocation range;
    size_t depth;
    size_t child_count;
    int expanded;
    int selected;
    int visible;
} UmiEditorDocumentOutlineEntry;

typedef struct UmiEditorDocumentOutline UmiEditorDocumentOutline;

UmiStatus umi_editor_document_outline_create(
    UmiEditorDocumentOutline **out_outline);
void umi_editor_document_outline_destroy(UmiEditorDocumentOutline *outline);
UmiStatus umi_editor_document_outline_build(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSymbolIndex *symbol_index,
    const char *document_id);
UmiStatus umi_editor_document_outline_select(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id);
UmiStatus umi_editor_document_outline_select_location(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSourceLocation *location);
UmiStatus umi_editor_document_outline_set_expanded(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id,
    int expanded);
UmiStatus umi_editor_document_outline_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t index,
    UmiEditorDocumentOutlineEntry *out_entry);
UmiStatus umi_editor_document_outline_visible_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t visible_index,
    UmiEditorDocumentOutlineEntry *out_entry);
size_t umi_editor_document_outline_count(
    const UmiEditorDocumentOutline *outline);
size_t umi_editor_document_outline_visible_count(
    const UmiEditorDocumentOutline *outline);
uint64_t umi_editor_document_outline_revision(
    const UmiEditorDocumentOutline *outline);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_DOCUMENT_OUTLINE_H */
