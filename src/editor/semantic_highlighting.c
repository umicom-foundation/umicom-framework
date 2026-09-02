/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/semantic_highlighting.c
 *
 * PURPOSE:
 *   Implement validated semantic token legends, transactional full and delta
 *   updates, deterministic ordering and frontend-neutral style resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/semantic_highlighting.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorSemanticHighlighting {
    char token_types[UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY]
                    [UMI_EDITOR_SEMANTIC_NAME_CAPACITY];
    char modifiers[UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY]
                  [UMI_EDITOR_SEMANTIC_NAME_CAPACITY];
    size_t token_type_count;
    size_t modifier_count;
    UmiEditorSemanticToken *tokens;
    size_t token_count;
    size_t token_capacity;
    UmiEditorSemanticStyle *styles;
    size_t style_count;
    size_t style_capacity;
    char document_uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char result_id[UMI_EDITOR_SEMANTIC_RESULT_ID_CAPACITY];
    uint64_t document_revision;
    uint64_t revision;
    int has_result;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the validate token operation used by this module and its client applications. */
static UmiStatus validate_token(const UmiEditorSemanticHighlighting *highlighting,
                                const UmiEditorSemanticToken *token)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || token == NULL ||
        token->struct_size != (uint32_t)sizeof(*token) ||
        token->api_version != UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION ||
        token->length == 0U || token->length > UINT64_MAX - token->column ||
        (size_t)token->type_index >= highlighting->token_type_count ||
        (highlighting->modifier_count < 64U &&
         (token->modifier_bits >> highlighting->modifier_count) != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the compare tokens operation used by this module and its client applications. */
static int compare_tokens(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorSemanticToken *left =
        (const UmiEditorSemanticToken *)left_pointer;
    const UmiEditorSemanticToken *right =
        (const UmiEditorSemanticToken *)right_pointer;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->line < right->line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->line > right->line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->column < right->column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->column > right->column) return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (left->length < right->length) return -1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (left->length > right->length) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->priority > right->priority) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->priority < right->priority) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->type_index < right->type_index) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->type_index > right->type_index) return 1;
    return 0;
}

/*
 * Provide the validate token array operation used by this module and its client
 * applications.
 */
static UmiStatus validate_token_array(
    const UmiEditorSemanticHighlighting *highlighting,
    UmiEditorSemanticToken *tokens,
    size_t token_count)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < token_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (validate_token(highlighting, &tokens[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_count > 1U) {
        qsort(tokens, token_count, sizeof(*tokens), compare_tokens);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < token_count; ++index) {
        const UmiEditorSemanticToken *previous = &tokens[index - 1U];
        const UmiEditorSemanticToken *current = &tokens[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (previous->line == current->line &&
            current->column < previous->column + previous->length) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve styles operation used by this module and its client applications. */
static UmiStatus reserve_styles(UmiEditorSemanticHighlighting *highlighting,
                                size_t required)
{
    size_t capacity;
    UmiEditorSemanticStyle *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= highlighting->style_capacity) return UMI_STATUS_OK;
    capacity = highlighting->style_capacity > 0U
        ? highlighting->style_capacity
        : 16U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSemanticStyle *)realloc(
        highlighting->styles, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    highlighting->styles = replacement;
    highlighting->style_capacity = capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise editor semantic highlighting from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_semantic_highlighting_create(
    UmiEditorSemanticHighlighting **out_highlighting)
{
    UmiEditorSemanticHighlighting *highlighting;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_highlighting == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_highlighting = NULL;
    highlighting = (UmiEditorSemanticHighlighting *)calloc(
        1U, sizeof(*highlighting));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    highlighting->revision = 1U;
    *out_highlighting = highlighting;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor semantic highlighting so the same storage can be
 * reused safely.
 */
void umi_editor_semantic_highlighting_destroy(
    UmiEditorSemanticHighlighting *highlighting)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL) return;
    free(highlighting->tokens);
    free(highlighting->styles);
    highlighting->tokens = NULL;
    highlighting->styles = NULL;
    free(highlighting);
}

/*
 * Release or reset state held by editor semantic highlighting so the same storage can be
 * reused safely.
 */
UmiStatus umi_editor_semantic_highlighting_clear(
    UmiEditorSemanticHighlighting *highlighting)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    highlighting->token_type_count = 0U;
    highlighting->modifier_count = 0U;
    highlighting->token_count = 0U;
    highlighting->style_count = 0U;
    highlighting->document_uri[0] = '\0';
    highlighting->result_id[0] = '\0';
    highlighting->document_revision = 0U;
    highlighting->has_result = 0;
    highlighting->revision = next_revision(highlighting->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting set legend operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_set_legend(
    UmiEditorSemanticHighlighting *highlighting,
    const char *const *token_types,
    size_t token_type_count,
    const char *const *modifiers,
    size_t modifier_count)
{
    char staged_types[UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY]
                     [UMI_EDITOR_SEMANTIC_NAME_CAPACITY] = {{0}};
    char staged_modifiers[UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY]
                         [UMI_EDITOR_SEMANTIC_NAME_CAPACITY] = {{0}};
    size_t index;
    size_t comparison;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || token_types == NULL || token_type_count == 0U ||
        token_type_count > UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY ||
        modifier_count > UMI_EDITOR_SEMANTIC_LEGEND_CAPACITY ||
        (modifier_count > 0U && modifiers == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < highlighting->style_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((size_t)highlighting->styles[index].type_index >= token_type_count ||
            (modifier_count < 64U &&
             (highlighting->styles[index].required_modifier_bits >>
              modifier_count) != 0U)) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < token_type_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (token_types[index] == NULL || token_types[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = copy_text(staged_types[index], sizeof(staged_types[index]),
                           token_types[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < index; ++comparison) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(staged_types[index], staged_types[comparison]) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < modifier_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (modifiers[index] == NULL || modifiers[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = copy_text(staged_modifiers[index],
                           sizeof(staged_modifiers[index]), modifiers[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < index; ++comparison) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(staged_modifiers[index], staged_modifiers[comparison]) ==
                0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    (void)memcpy(highlighting->token_types, staged_types,
                 sizeof(staged_types));
    (void)memcpy(highlighting->modifiers, staged_modifiers,
                 sizeof(staged_modifiers));
    highlighting->token_type_count = token_type_count;
    highlighting->modifier_count = modifier_count;
    highlighting->token_count = 0U;
    highlighting->has_result = 0;
    highlighting->result_id[0] = '\0';
    highlighting->revision = next_revision(highlighting->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting upsert style operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_upsert_style(
    UmiEditorSemanticHighlighting *highlighting,
    const UmiEditorSemanticStyle *style)
{
    UmiEditorSemanticStyle stored;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || style == NULL ||
        style->struct_size != (uint32_t)sizeof(*style) ||
        style->api_version != UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION ||
        (size_t)style->type_index >= highlighting->token_type_count ||
        !terminated(style->foreground_role, sizeof(style->foreground_role)) ||
        style->foreground_role[0] == '\0' ||
        (highlighting->modifier_count < 64U &&
         (style->required_modifier_bits >> highlighting->modifier_count) != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < highlighting->style_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (highlighting->styles[index].type_index == style->type_index &&
            highlighting->styles[index].required_modifier_bits ==
                style->required_modifier_bits) {
            break;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == highlighting->style_count) {
        status = reserve_styles(highlighting, highlighting->style_count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        ++highlighting->style_count;
    }
    stored = *style;
    stored.bold = style->bold != 0;
    stored.italic = style->italic != 0;
    stored.underline = style->underline != 0;
    stored.strikethrough = style->strikethrough != 0;
    highlighting->styles[index] = stored;
    highlighting->revision = next_revision(highlighting->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting replace operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_semantic_highlighting_replace(
    UmiEditorSemanticHighlighting *highlighting,
    const char *document_uri,
    uint64_t document_revision,
    const char *result_id,
    const UmiEditorSemanticToken *tokens,
    size_t token_count)
{
    UmiEditorSemanticToken *staged = NULL;
    char staged_document_uri[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char staged_result_id[UMI_EDITOR_SEMANTIC_RESULT_ID_CAPACITY];
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || document_uri == NULL ||
        document_uri[0] == '\0' || result_id == NULL || result_id[0] == '\0' ||
        (token_count > 0U && tokens == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_count > SIZE_MAX / sizeof(*staged)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token_count > 0U) {
        staged = (UmiEditorSemanticToken *)malloc(token_count * sizeof(*staged));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        (void)memcpy(staged, tokens, token_count * sizeof(*staged));
    }
    status = copy_text(staged_document_uri, sizeof(staged_document_uri),
                       document_uri);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(staged_result_id, sizeof(staged_result_id),
                           result_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = validate_token_array(highlighting, staged, token_count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(staged);
        return status;
    }
    free(highlighting->tokens);
    highlighting->tokens = staged;
    highlighting->token_count = token_count;
    highlighting->token_capacity = token_count;
    (void)memcpy(highlighting->document_uri, staged_document_uri,
                 sizeof(staged_document_uri));
    (void)memcpy(highlighting->result_id, staged_result_id,
                 sizeof(staged_result_id));
    highlighting->document_revision = document_revision;
    highlighting->has_result = 1;
    highlighting->revision = next_revision(highlighting->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting apply delta operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_apply_delta(
    UmiEditorSemanticHighlighting *highlighting,
    const char *expected_result_id,
    const UmiEditorSemanticTokenDelta *delta,
    const char *next_result_id,
    uint64_t document_revision)
{
    UmiEditorSemanticToken *staged = NULL;
    size_t retained_count;
    size_t next_count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || !highlighting->has_result ||
        expected_result_id == NULL || next_result_id == NULL ||
        next_result_id[0] == '\0' || delta == NULL ||
        delta->struct_size != (uint32_t)sizeof(*delta) ||
        delta->api_version != UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION ||
        strcmp(highlighting->result_id, expected_result_id) != 0 ||
        delta->start > highlighting->token_count ||
        delta->delete_count > highlighting->token_count - delta->start ||
        (delta->token_count > 0U && delta->tokens == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    retained_count = highlighting->token_count - delta->delete_count;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (delta->token_count > SIZE_MAX - retained_count) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next_count = retained_count + delta->token_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next_count > SIZE_MAX / sizeof(*staged)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (next_count > 0U) {
        staged = (UmiEditorSemanticToken *)malloc(next_count * sizeof(*staged));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (delta->start > 0U && staged != NULL) {
        (void)memcpy(staged, highlighting->tokens,
                     delta->start * sizeof(*staged));
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (delta->token_count > 0U && staged != NULL) {
        (void)memcpy(&staged[delta->start], delta->tokens,
                     delta->token_count * sizeof(*staged));
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (delta->start + delta->delete_count < highlighting->token_count &&
        staged != NULL) {
        size_t source = delta->start + delta->delete_count;
        size_t destination = delta->start + delta->token_count;
        size_t tail_count = highlighting->token_count - source;
        (void)memcpy(&staged[destination], &highlighting->tokens[source],
                     tail_count * sizeof(*staged));
    }
    status = validate_token_array(highlighting, staged, next_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(highlighting->result_id,
                           sizeof(highlighting->result_id), next_result_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(staged);
        return status;
    }
    free(highlighting->tokens);
    highlighting->tokens = staged;
    highlighting->token_count = next_count;
    highlighting->token_capacity = next_count;
    highlighting->document_revision = document_revision;
    highlighting->revision = next_revision(highlighting->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor semantic highlighting while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_semantic_highlighting_at(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    UmiEditorSemanticToken *out_token)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || out_token == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= highlighting->token_count) return UMI_STATUS_NOT_FOUND;
    *out_token = highlighting->tokens[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the modifier specificity operation used by this module and its client
 * applications.
 */
static size_t modifier_specificity(uint64_t bits)
{
    size_t count = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (bits != 0U) {
        count += (size_t)(bits & 1U);
        bits >>= 1U;
    }
    return count;
}

/*
 * Provide the editor semantic highlighting style for token operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_style_for_token(
    const UmiEditorSemanticHighlighting *highlighting,
    const UmiEditorSemanticToken *token,
    UmiEditorSemanticStyle *out_style)
{
    size_t index;
    size_t selected = SIZE_MAX;
    size_t selected_specificity = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || out_style == NULL ||
        validate_token(highlighting, token) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < highlighting->style_count; ++index) {
        const UmiEditorSemanticStyle *style = &highlighting->styles[index];
        size_t specificity;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (style->type_index != token->type_index ||
            (token->modifier_bits & style->required_modifier_bits) !=
                style->required_modifier_bits) {
            continue;
        }
        specificity = modifier_specificity(style->required_modifier_bits);
        /* Apply this branch only when its contract condition is satisfied. */
        if (selected == SIZE_MAX || specificity > selected_specificity) {
            selected = index;
            selected_specificity = specificity;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_style = highlighting->styles[selected];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_semantic_highlighting_snapshot(
    const UmiEditorSemanticHighlighting *highlighting,
    UmiEditorSemanticHighlightingSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION;
    (void)copy_text(out_snapshot->document_uri,
                    sizeof(out_snapshot->document_uri),
                    highlighting->document_uri);
    (void)copy_text(out_snapshot->result_id, sizeof(out_snapshot->result_id),
                    highlighting->result_id);
    out_snapshot->token_type_count = highlighting->token_type_count;
    out_snapshot->modifier_count = highlighting->modifier_count;
    out_snapshot->token_count = highlighting->token_count;
    out_snapshot->style_count = highlighting->style_count;
    out_snapshot->document_revision = highlighting->document_revision;
    out_snapshot->revision = highlighting->revision;
    out_snapshot->has_result = highlighting->has_result;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting token type name operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_token_type_name(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    const char **out_name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || out_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= highlighting->token_type_count) return UMI_STATUS_NOT_FOUND;
    *out_name = highlighting->token_types[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor semantic highlighting modifier name operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_semantic_highlighting_modifier_name(
    const UmiEditorSemanticHighlighting *highlighting,
    size_t index,
    const char **out_name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (highlighting == NULL || out_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= highlighting->modifier_count) return UMI_STATUS_NOT_FOUND;
    *out_name = highlighting->modifiers[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor semantic highlighting without
 * changing their state.
 */
size_t umi_editor_semantic_highlighting_count(
    const UmiEditorSemanticHighlighting *highlighting)
{
    return highlighting != NULL ? highlighting->token_count : 0U;
}

/*
 * Provide the editor semantic highlighting revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_semantic_highlighting_revision(
    const UmiEditorSemanticHighlighting *highlighting)
{
    return highlighting != NULL ? highlighting->revision : 0U;
}
