/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/quick_documentation.h
 *
 * PURPOSE:
 *   Model structured quick documentation assembled from native analysers,
 *   language servers, indexed manuals, RAG and future AI providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_QUICK_DOCUMENTATION_H
#define UMICOM_EDITOR_QUICK_DOCUMENTATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_QUICK_DOCUMENTATION_API_VERSION 1U
#define UMI_EDITOR_DOCUMENTATION_ID_CAPACITY 128U
#define UMI_EDITOR_DOCUMENTATION_SYMBOL_CAPACITY 128U
#define UMI_EDITOR_DOCUMENTATION_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_DOCUMENTATION_TITLE_CAPACITY 256U
#define UMI_EDITOR_DOCUMENTATION_LANGUAGE_CAPACITY 64U
#define UMI_EDITOR_DOCUMENTATION_CONTENT_CAPACITY 8192U
#define UMI_EDITOR_DOCUMENTATION_URI_CAPACITY 1024U

typedef enum UmiEditorQuickDocumentationSectionKind {
    UMI_EDITOR_DOCUMENTATION_SUMMARY = 1,
    UMI_EDITOR_DOCUMENTATION_SIGNATURE = 2,
    UMI_EDITOR_DOCUMENTATION_PARAMETERS = 3,
    UMI_EDITOR_DOCUMENTATION_RETURNS = 4,
    UMI_EDITOR_DOCUMENTATION_EXAMPLE = 5,
    UMI_EDITOR_DOCUMENTATION_NOTE = 6,
    UMI_EDITOR_DOCUMENTATION_LINK = 7
} UmiEditorQuickDocumentationSectionKind;

typedef enum UmiEditorQuickDocumentationContentKind {
    UMI_EDITOR_DOCUMENTATION_PLAIN_TEXT = 1,
    UMI_EDITOR_DOCUMENTATION_MARKDOWN = 2,
    UMI_EDITOR_DOCUMENTATION_SOURCE_CODE = 3
} UmiEditorQuickDocumentationContentKind;

typedef struct UmiEditorQuickDocumentationSection {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_DOCUMENTATION_ID_CAPACITY];
    char symbol_id[UMI_EDITOR_DOCUMENTATION_SYMBOL_CAPACITY];
    char provider_id[UMI_EDITOR_DOCUMENTATION_PROVIDER_CAPACITY];
    char title[UMI_EDITOR_DOCUMENTATION_TITLE_CAPACITY];
    char language_id[UMI_EDITOR_DOCUMENTATION_LANGUAGE_CAPACITY];
    char content[UMI_EDITOR_DOCUMENTATION_CONTENT_CAPACITY];
    char source_uri[UMI_EDITOR_DOCUMENTATION_URI_CAPACITY];
    UmiEditorSourceLocation location;
    UmiEditorQuickDocumentationSectionKind section_kind;
    UmiEditorQuickDocumentationContentKind content_kind;
    int32_t order;
    int trusted;
    int visible;
} UmiEditorQuickDocumentationSection;

typedef struct UmiEditorQuickDocumentationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t section_count;
    size_t selected_section_count;
    size_t trusted_section_count;
    size_t link_section_count;
    uint64_t revision;
    char selected_symbol_id[UMI_EDITOR_DOCUMENTATION_SYMBOL_CAPACITY];
    int pinned;
} UmiEditorQuickDocumentationSnapshot;

typedef struct UmiEditorQuickDocumentation UmiEditorQuickDocumentation;

UmiStatus umi_editor_quick_documentation_create(
    UmiEditorQuickDocumentation **out_documentation);
void umi_editor_quick_documentation_destroy(
    UmiEditorQuickDocumentation *documentation);
UmiStatus umi_editor_quick_documentation_clear(
    UmiEditorQuickDocumentation *documentation);
UmiStatus umi_editor_quick_documentation_upsert(
    UmiEditorQuickDocumentation *documentation,
    const UmiEditorQuickDocumentationSection *section);
UmiStatus umi_editor_quick_documentation_remove(
    UmiEditorQuickDocumentation *documentation,
    const char *section_id);
UmiStatus umi_editor_quick_documentation_remove_symbol(
    UmiEditorQuickDocumentation *documentation,
    const char *symbol_id);
UmiStatus umi_editor_quick_documentation_sort(
    UmiEditorQuickDocumentation *documentation);
UmiStatus umi_editor_quick_documentation_select_symbol(
    UmiEditorQuickDocumentation *documentation,
    const char *symbol_id);
UmiStatus umi_editor_quick_documentation_set_pinned(
    UmiEditorQuickDocumentation *documentation,
    int pinned);
UmiStatus umi_editor_quick_documentation_at(
    const UmiEditorQuickDocumentation *documentation,
    size_t index,
    UmiEditorQuickDocumentationSection *out_section);
UmiStatus umi_editor_quick_documentation_selected_at(
    const UmiEditorQuickDocumentation *documentation,
    size_t selected_index,
    UmiEditorQuickDocumentationSection *out_section);
UmiStatus umi_editor_quick_documentation_snapshot(
    const UmiEditorQuickDocumentation *documentation,
    UmiEditorQuickDocumentationSnapshot *out_snapshot);
size_t umi_editor_quick_documentation_count(
    const UmiEditorQuickDocumentation *documentation);
size_t umi_editor_quick_documentation_symbol_count(
    const UmiEditorQuickDocumentation *documentation,
    const char *symbol_id);
uint64_t umi_editor_quick_documentation_revision(
    const UmiEditorQuickDocumentation *documentation);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_QUICK_DOCUMENTATION_H */
