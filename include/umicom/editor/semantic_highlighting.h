/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/semantic_highlighting.h
 *
 * PURPOSE:
 *   Define provider-neutral semantic token legends, full results, incremental
 *   deltas and toolkit-neutral presentation roles for every editor frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SEMANTIC_HIGHLIGHTING_H
#define UMICOM_EDITOR_SEMANTIC_HIGHLIGHTING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION 1U
#define UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY 64U
#define UMI_EDITOR_SEMANTIC_NAME_CAPACITY 64U
#define UMI_EDITOR_SEMANTIC_ROLE_CAPACITY 96U
#define UMI_EDITOR_SEMANTIC_RESULT_ID_CAPACITY 128U

typedef struct UmiEditorSemanticToken {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t line;
    uint64_t column;
    uint64_t length;
    uint32_t type_index;
    uint64_t modifier_bits;
    int32_t priority;
} UmiEditorSemanticToken;

typedef struct UmiEditorSemanticStyle {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t type_index;
    uint64_t required_modifier_bits;
    char foreground_role[UMI_EDITOR_SEMANTIC_ROLE_CAPACITY];
    int bold;
    int italic;
    int underline;
    int strikethrough;
} UmiEditorSemanticStyle;

typedef struct UmiEditorSemanticTokenDelta {
    uint32_t struct_size;
    uint32_t api_version;
    size_t start;
    size_t delete_count;
    const UmiEditorSemanticToken *tokens;
    size_t token_count;
} UmiEditorSemanticTokenDelta;

typedef struct UmiEditorSemanticHighlightingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char document_uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char result_id[UMI_EDITOR_SEMANTIC_RESULT_ID_CAPACITY];
    size_t token_type_count;
    size_t modifier_count;
    size_t token_count;
    size_t style_count;
    uint64_t document_revision;
    uint64_t revision;
    int has_result;
} UmiEditorSemanticHighlightingSnapshot;

typedef struct UmiEditorSemanticHighlighting UmiEditorSemanticHighlighting;

UmiStatus umi_editor_semantic_highlighting_create(
    UmiEditorSemanticHighlighting **out_highlighting);
void umi_editor_semantic_highlighting_destroy(
    UmiEditorSemanticHighlighting *highlighting);
UmiStatus umi_editor_semantic_highlighting_clear(
    UmiEditorSemanticHighlighting *highlighting);
UmiStatus umi_editor_semantic_highlighting_set_legend(
    UmiEditorSemanticHighlighting *highlighting,
    const char *const *token_types,
    size_t token_type_count,
    const char *const *modifiers,
    size_t modifier_count);
UmiStatus umi_editor_semantic_highlighting_upsert_style(
    UmiEditorSemanticHighlighting *highlighting,
    const UmiEditorSemanticStyle *style);
UmiStatus umi_editor_semantic_highlighting_replace(
    UmiEditorSemanticHighlighting *highlighting,
    const char *document_uri,
    uint64_t document_revision,
    const char *result_id,
    const UmiEditorSemanticToken *tokens,
    size_t token_count);
UmiStatus umi_editor_semantic_highlighting_apply_delta(
    UmiEditorSemanticHighlighting *highlighting,
    const char *expected_result_id,
    const UmiEditorSemanticTokenDelta *delta,
    const char *next_result_id,
    uint64_t document_revision);
UmiStatus umi_editor_semantic_highlighting_at(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    UmiEditorSemanticToken *out_token);
UmiStatus umi_editor_semantic_highlighting_style_for_token(
    const UmiEditorSemanticHighlighting *highlighting,
    const UmiEditorSemanticToken *token,
    UmiEditorSemanticStyle *out_style);
UmiStatus umi_editor_semantic_highlighting_snapshot(
    const UmiEditorSemanticHighlighting *highlighting,
    UmiEditorSemanticHighlightingSnapshot *out_snapshot);
UmiStatus umi_editor_semantic_highlighting_token_type_name(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    const char **out_name);
UmiStatus umi_editor_semantic_highlighting_modifier_name(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    const char **out_name);
size_t umi_editor_semantic_highlighting_count(
    const UmiEditorSemanticHighlighting *highlighting);
uint64_t umi_editor_semantic_highlighting_revision(
    const UmiEditorSemanticHighlighting *highlighting);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SEMANTIC_HIGHLIGHTING_H */
