/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_semantic_highlighting.c
 *
 * PURPOSE:
 *   Implement the test editor semantic highlighting behavior for
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
 * File: tests/editor/test_editor_semantic_highlighting.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/semantic_highlighting.h"

static UmiEditorSemanticToken make_token(uint64_t column,
                                         uint64_t length,
                                         uint32_t type_index,
                                         uint64_t modifiers)
{
    UmiEditorSemanticToken token = {0};
    token.struct_size = (uint32_t)sizeof(token);
    token.api_version = UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION;
    token.line = 0U;
    token.column = column;
    token.length = length;
    token.type_index = type_index;
    token.modifier_bits = modifiers;
    return token;
}

int main(void)
{
    const char *types[] = {"keyword", "variable"};
    const char *modifiers[] = {"declaration", "readonly"};
    UmiEditorSemanticHighlighting *highlighting = NULL;
    UmiEditorSemanticToken tokens[2];
    UmiEditorSemanticToken inserted;
    UmiEditorSemanticToken token;
    UmiEditorSemanticTokenDelta delta = {0};
    UmiEditorSemanticStyle style = {0};
    UmiEditorSemanticStyle resolved_style;
    UmiEditorSemanticHighlightingSnapshot snapshot;
    const char *name = NULL;

    assert(umi_editor_semantic_highlighting_create(&highlighting) ==
           UMI_STATUS_OK);
    assert(umi_editor_semantic_highlighting_set_legend(
               highlighting, types, 2U, modifiers, 2U) == UMI_STATUS_OK);
    style.struct_size = (uint32_t)sizeof(style);
    style.api_version = UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION;
    style.type_index = 1U;
    (void)strcpy(style.foreground_role, "editor.semantic.variable");
    assert(umi_editor_semantic_highlighting_upsert_style(highlighting, &style) ==
           UMI_STATUS_OK);
    style.required_modifier_bits = 1U << 1U;
    style.bold = 1;
    (void)strcpy(style.foreground_role, "editor.semantic.readonly-variable");
    assert(umi_editor_semantic_highlighting_upsert_style(highlighting, &style) ==
           UMI_STATUS_OK);

    tokens[0] = make_token(4U, 5U, 1U, 1U << 1U);
    tokens[1] = make_token(0U, 3U, 0U, 0U);
    assert(umi_editor_semantic_highlighting_replace(
               highlighting, "file:///workspace/main.c", 7U, "result-1",
               tokens, 2U) == UMI_STATUS_OK);
    assert(umi_editor_semantic_highlighting_at(highlighting, 0U, &token) ==
           UMI_STATUS_OK);
    assert(token.type_index == 0U);
    assert(umi_editor_semantic_highlighting_at(highlighting, 1U, &token) ==
           UMI_STATUS_OK);
    assert(umi_editor_semantic_highlighting_style_for_token(
               highlighting, &token, &resolved_style) == UMI_STATUS_OK);
    assert(strcmp(resolved_style.foreground_role,
                  "editor.semantic.readonly-variable") == 0);

    inserted = make_token(10U, 1U, 1U, 0U);
    delta.struct_size = (uint32_t)sizeof(delta);
    delta.api_version = UMI_EDITOR_SEMANTIC_HIGHLIGHTING_API_VERSION;
    delta.start = 2U;
    delta.tokens = &inserted;
    delta.token_count = 1U;
    assert(umi_editor_semantic_highlighting_apply_delta(
               highlighting, "result-1", &delta, "result-2", 8U) ==
           UMI_STATUS_OK);
    assert(umi_editor_semantic_highlighting_snapshot(highlighting, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.token_count == 3U);
    assert(snapshot.document_revision == 8U);
    assert(strcmp(snapshot.result_id, "result-2") == 0);
    assert(umi_editor_semantic_highlighting_token_type_name(
               highlighting, 1U, &name) == UMI_STATUS_OK);
    assert(strcmp(name, "variable") == 0);
    umi_editor_semantic_highlighting_destroy(highlighting);
    return 0;
}
