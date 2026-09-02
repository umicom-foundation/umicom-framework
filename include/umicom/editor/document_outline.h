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

/**
 * Represent the editor document outline entry data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor document outline data shared with callers of this public contract.
 */
typedef struct UmiEditorDocumentOutline UmiEditorDocumentOutline;

/**
 * Initialise editor document outline from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_document_outline_create(
    UmiEditorDocumentOutline **out_outline);
/**
 * Release or reset state held by editor document outline so the same storage can be reused
 * safely.
 */
void umi_editor_document_outline_destroy(UmiEditorDocumentOutline *outline);
/**
 * Provide the editor document outline build operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_document_outline_build(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSymbolIndex *symbol_index,
    const char *document_id);
/**
 * Provide the editor document outline select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_document_outline_select(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id);
/**
 * Provide the editor document outline select location operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_document_outline_select_location(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSourceLocation *location);
/**
 * Provide the editor document outline set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_document_outline_set_expanded(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id,
    int expanded);
/**
 * Find editor document outline entry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_document_outline_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t index,
    UmiEditorDocumentOutlineEntry *out_entry);
/**
 * Find editor document outline visible entry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_document_outline_visible_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t visible_index,
    UmiEditorDocumentOutlineEntry *out_entry);
/**
 * Return the number of records represented by editor document outline without changing
 * their state.
 */
size_t umi_editor_document_outline_count(
    const UmiEditorDocumentOutline *outline);
/**
 * Return the number of records represented by editor document outline visible without
 * changing their state.
 */
size_t umi_editor_document_outline_visible_count(
    const UmiEditorDocumentOutline *outline);
/**
 * Provide the editor document outline revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_document_outline_revision(
    const UmiEditorDocumentOutline *outline);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_DOCUMENT_OUTLINE_H */
