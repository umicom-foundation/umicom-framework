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

/**
 * Represent the editor semantic token data shared with callers of this public contract.
 */
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

/**
 * Represent the editor semantic style data shared with callers of this public contract.
 */
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

/**
 * Represent the editor semantic token delta data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSemanticTokenDelta {
    uint32_t struct_size;
    uint32_t api_version;
    size_t start;
    size_t delete_count;
    const UmiEditorSemanticToken *tokens;
    size_t token_count;
} UmiEditorSemanticTokenDelta;

/**
 * Represent the editor semantic highlighting snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor semantic highlighting data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorSemanticHighlighting UmiEditorSemanticHighlighting;

/**
 * Initialise editor semantic highlighting from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_semantic_highlighting_create(
    UmiEditorSemanticHighlighting **out_highlighting);
/**
 * Release or reset state held by editor semantic highlighting so the same storage can be
 * reused safely.
 */
void umi_editor_semantic_highlighting_destroy(
    UmiEditorSemanticHighlighting *highlighting);
/**
 * Release or reset state held by editor semantic highlighting so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_semantic_highlighting_clear(
    UmiEditorSemanticHighlighting *highlighting);
/**
 * Provide the editor semantic highlighting set legend operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_set_legend(
    UmiEditorSemanticHighlighting *highlighting,
    const char *const *token_types,
    size_t token_type_count,
    const char *const *modifiers,
    size_t modifier_count);
/**
 * Provide the editor semantic highlighting upsert style operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_upsert_style(
    UmiEditorSemanticHighlighting *highlighting,
    const UmiEditorSemanticStyle *style);
/**
 * Provide the editor semantic highlighting replace operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_semantic_highlighting_replace(
    UmiEditorSemanticHighlighting *highlighting,
    const char *document_uri,
    uint64_t document_revision,
    const char *result_id,
    const UmiEditorSemanticToken *tokens,
    size_t token_count);
/**
 * Provide the editor semantic highlighting apply delta operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_apply_delta(
    UmiEditorSemanticHighlighting *highlighting,
    const char *expected_result_id,
    const UmiEditorSemanticTokenDelta *delta,
    const char *next_result_id,
    uint64_t document_revision);
/**
 * Find editor semantic highlighting while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_semantic_highlighting_at(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    UmiEditorSemanticToken *out_token);
/**
 * Provide the editor semantic highlighting style for token operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_style_for_token(
    const UmiEditorSemanticHighlighting *highlighting,
    const UmiEditorSemanticToken *token,
    UmiEditorSemanticStyle *out_style);
/**
 * Provide the editor semantic highlighting snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_semantic_highlighting_snapshot(
    const UmiEditorSemanticHighlighting *highlighting,
    UmiEditorSemanticHighlightingSnapshot *out_snapshot);
/**
 * Provide the editor semantic highlighting token type name operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_token_type_name(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    const char **out_name);
/**
 * Provide the editor semantic highlighting modifier name operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_modifier_name(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    const char **out_name);
/**
 * Return the number of records represented by editor semantic highlighting without
 * changing their state.
 */
size_t umi_editor_semantic_highlighting_count(
    const UmiEditorSemanticHighlighting *highlighting);
/**
 * Provide the editor semantic highlighting revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_semantic_highlighting_revision(
    const UmiEditorSemanticHighlighting *highlighting);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SEMANTIC_HIGHLIGHTING_H */
