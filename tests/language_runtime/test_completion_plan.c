/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_completion_plan.c
 * PURPOSE: Check accepted completion text and atomic refusal of stale or unsafe edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        if (!(condition))                                                                                    \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #condition);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
#define RANGE "{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":2}}"
#define INSERT_RANGE "{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":1}}"
#define IMPORT_RANGE "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":0}}"
#define ITEM_EDIT "\"textEdit\":{\"range\":" RANGE ",\"newText\":\"puts\"}"
typedef struct PlanCase
{
    const char *name, *json, *after;
    UmiStatus status;
} PlanCase;
static const PlanCase cases[] = {
    {"label", "[{\"label\":\"puts\"}]", "//\nputs\n", UMI_STATUS_OK},
    {"insert-text", "[{\"label\":\"show\",\"insertText\":\"puts\"}]", "//\nputs\n", UMI_STATUS_OK},
    {"text-edit", "[{\"label\":\"show\",\"insertText\":\"ignored\"," ITEM_EDIT "}]", "//\nputs\n",
     UMI_STATUS_OK},
    {"delete", "[{\"label\":\"remove\",\"insertText\":\"\"}]", "//\n\n", UMI_STATUS_OK},
    {"additional",
     "[{\"label\":\"puts\"," ITEM_EDIT ",\"additionalTextEdits\":[{\"range\":" IMPORT_RANGE
     ",\"newText\":\"#include <stdio.h>\\n\"}]}]",
     "#include <stdio.h>\n//\nputs\n", UMI_STATUS_OK},
    {"insert-choice",
     "[{\"label\":\"show\",\"textEdit\":{\"insert\":" INSERT_RANGE ",\"replace\":" RANGE
     ",\"newText\":\"puts\"}}]",
     "//\nputsb\n", UMI_STATUS_OK},
    {"replace-choice",
     "[{\"label\":\"show\",\"textEdit\":{\"insert\":" INSERT_RANGE ",\"replace\":" RANGE
     ",\"newText\":\"puts\"}}]",
     "//\nputs\n", UMI_STATUS_OK},
    {"default-range",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"editRange\":" RANGE
     "},\"items\":[{\"label\":\"show\",\"textEditText\":\"puts\"}]}",
     "//\nputs\n", UMI_STATUS_OK},
    {"default-label",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"editRange\":" RANGE
     "},\"items\":[{\"label\":\"puts\",\"insertText\":\"ignored\"}]}",
     "//\nputs\n", UMI_STATUS_OK},
    {"default-empty",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"editRange\":" RANGE
     "},\"items\":[{\"label\":\"puts\",\"textEditText\":\"\"}]}",
     "//\n\n", UMI_STATUS_OK},
    {"default-pair",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"editRange\":{\"insert\":" INSERT_RANGE ",\"replace\":" RANGE
     "}},\"items\":[{\"label\":\"puts\"}]}",
     "//\nputs\n", UMI_STATUS_OK},
    {"item-override",
     "{\"isIncomplete\":false,\"itemDefaults\":{\"editRange\":" IMPORT_RANGE
     ",\"insertTextFormat\":2},\"items\":[{\"label\":\"show\",\"insertTextFormat\":1," ITEM_EDIT "}]}",
     "//\nputs\n", UMI_STATUS_OK},
    {"snippet", "[{\"label\":\"function\",\"insertText\":\"f($1)\",\"insertTextFormat\":2}]", NULL,
     UMI_STATUS_NOT_IMPLEMENTED},
    {"indent", "[{\"label\":\"puts\",\"insertTextMode\":2}]", NULL, UMI_STATUS_NOT_IMPLEMENTED},
    {"command", "[{\"label\":\"puts\",\"command\":{\"title\":\"Run\",\"command\":\"do.work\"}}]", NULL,
     UMI_STATUS_NOT_IMPLEMENTED},
    {"overlap",
     "[{\"label\":\"puts\"," ITEM_EDIT ",\"additionalTextEdits\":[{\"range\":" INSERT_RANGE
     ",\"newText\":\"x\"}]}]",
     NULL, UMI_STATUS_INVALID_STATE},
    {"duplicate-insert",
     "[{\"label\":\"puts\"," ITEM_EDIT ",\"additionalTextEdits\":[{\"range\":" IMPORT_RANGE
     ",\"newText\":\"a\"},{\"range\":" IMPORT_RANGE ",\"newText\":\"b\"}]}]",
     NULL, UMI_STATUS_INVALID_STATE},
    {"invalid-additional",
     "[{\"label\":\"puts\"," ITEM_EDIT ",\"additionalTextEdits\":[{\"range\":" IMPORT_RANGE
     ",\"newText\":true}]}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"multiline",
     "[{\"label\":\"puts\",\"textEdit\":{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":"
     "1,\"character\":2}},\"newText\":\"x\"}}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"outside", "[{\"label\":\"puts\",\"textEdit\":{\"range\":" IMPORT_RANGE ",\"newText\":\"x\"}}]", NULL,
     UMI_STATUS_PARSE_ERROR},
    {"negative",
     "[{\"label\":\"puts\",\"textEdit\":{\"range\":{\"start\":{\"line\":1,\"character\":-1},\"end\":{"
     "\"line\":1,\"character\":2}},\"newText\":\"x\"}}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"fraction",
     "[{\"label\":\"puts\",\"textEdit\":{\"range\":{\"start\":{\"line\":1,\"character\":0.0},\"end\":{"
     "\"line\":1,\"character\":2}},\"newText\":\"x\"}}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"reversed",
     "[{\"label\":\"puts\",\"textEdit\":{\"range\":{\"start\":{\"line\":1,\"character\":2},\"end\":{\"line\":"
     "1,\"character\":0}},\"newText\":\"x\"}}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"bad-prefix",
     "[{\"label\":\"puts\",\"textEdit\":{\"insert\":" RANGE ",\"replace\":" INSERT_RANGE
     ",\"newText\":\"x\"}}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"ambiguous",
     "[{\"label\":\"puts\",\"textEdit\":{\"range\":" RANGE ",\"insert\":" INSERT_RANGE ",\"replace\":" RANGE
     ",\"newText\":\"x\"}}]",
     NULL, UMI_STATUS_PARSE_ERROR},
    {"missing-text", "[{\"label\":\"puts\",\"textEdit\":{\"range\":" RANGE "}}]", NULL,
     UMI_STATUS_PARSE_ERROR},
    {"duplicate-range",
     "[{\"label\":\"puts\",\"textEdit\":{\"range\":" RANGE ",\"range\":" RANGE ",\"newText\":\"x\"}}]", NULL,
     UMI_STATUS_ALREADY_EXISTS}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1], *json = "[{\"label\":\"puts\"}]";
    const char *before = "//\nab\n", *after = "//\nputs\n";
    UmiStatus expected = UMI_STATUS_OK;
    size_t chosen;
    for (chosen = 0U; chosen < sizeof(cases) / sizeof(cases[0]); ++chosen)
        if (strcmp(name, cases[chosen].name) == 0)
            break;
    if (chosen < sizeof(cases) / sizeof(cases[0]))
    {
        json = cases[chosen].json;
        after = cases[chosen].after;
        expected = cases[chosen].status;
    }
    UmiEditorTextBuffer *buffer = NULL;
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    UmiEditorWorkspaceEditSet *edits = NULL;
    UmiCancellationToken *cancel = NULL;
    UmiLanguageCompletionContext context = {
        "file:///project/main.c",       "test-language", 0U, {1U, 1U}, {{1U, 0U}, {1U, 2U}},
        UMI_LANGUAGE_COMPLETION_REPLACE};
    char dynamic[18000], long_source[520];
    if (strcmp(name, "insert-choice") == 0)
        context.acceptance = UMI_LANGUAGE_COMPLETION_INSERT;
    if (strcmp(name, "unicode") == 0 || strcmp(name, "surrogate") == 0 ||
        strcmp(name, "cursor-surrogate") == 0)
    {
        before = "//\n\xf0\x9f\x98\x80"
                 "ab\n";
        after = "//\n\xf0\x9f\x98\x80"
                "puts\n";
        context.request_position.character = 3U;
        context.fallback_range.start.character = strcmp(name, "surrogate") == 0 ? 1U : 2U;
        context.fallback_range.end.character = 4U;
        if (strcmp(name, "surrogate") == 0)
            expected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(name, "cursor-surrogate") == 0)
        {
            context.request_position.character = 1U;
            context.fallback_range.start.character = 0U;
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    else if (strcmp(name, "cr") == 0)
    {
        before = "//\rab\r";
        after = "//\rputs\r";
    }
    else if (strcmp(name, "crlf") == 0)
    {
        before = "//\r\nab\r\n";
        after = "//\r\nputs\r\n";
    }
    else if (strcmp(name, "replacement-limit") == 0)
    {
        memcpy(dynamic, "[{\"label\":\"x\",\"insertText\":\"", 28U);
        memset(dynamic + 28U, 'x', 512U);
        memcpy(dynamic + 540U, "\"}]", 4U);
        json = dynamic;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(name, "expected-limit") == 0)
    {
        memcpy(long_source, "//\n", 3U);
        memset(long_source + 3U, 'a', 512U);
        long_source[515] = '\n';
        long_source[516] = '\0';
        before = long_source;
        context.fallback_range.end.character = 512U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(name, "additional-limit") == 0)
    {
        int written = snprintf(dynamic, sizeof(dynamic), "[{\"label\":\"x\",\"additionalTextEdits\":[");
        CHECK(written > 0);
        size_t at = (size_t)written;
        for (size_t i = 0U; i < 65U; ++i)
        {
            written = snprintf(dynamic + at, sizeof(dynamic) - at,
                               "%s{\"range\":" IMPORT_RANGE ",\"newText\":\"x\"}", i == 0U ? "" : ",");
            CHECK(written > 0 && (size_t)written < sizeof(dynamic) - at);
            at += (size_t)written;
        }
        CHECK(snprintf(dynamic + at, sizeof(dynamic) - at, "]}]") == 3);
        json = dynamic;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(name, "fallback-outside") == 0)
    {
        context.fallback_range.end.character = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(name, "invalid-mode") == 0)
    {
        context.acceptance = (UmiLanguageCompletionAcceptance)0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(name, "position-missing") == 0)
    {
        context.request_position.character = 9U;
        context.fallback_range.end.character = 10U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(name, "cancel") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    else if (strcmp(name, "stale-plan") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    else if (chosen == sizeof(cases) / sizeof(cases[0]))
        CHECK(strcmp(name, "stale-apply") == 0);
    CHECK(umi_editor_text_buffer_create(64U, &buffer) == UMI_STATUS_OK);
    CHECK(umi_editor_text_buffer_set(buffer, before, strlen(before)) == UMI_STATUS_OK);
    context.request_revision = umi_editor_text_buffer_revision(buffer);
    if (strcmp(name, "stale-plan") == 0)
        --context.request_revision;
    CHECK(UmiLanguageCompletionCatalogueCreate(json, strlen(json), NULL, &catalogue) == UMI_STATUS_OK);
    CHECK(UmiLanguageCompletionPlanCreate(catalogue, 0U, &context, buffer, cancel, &edits) == expected);
    UmiEditorTextBufferView view;
    CHECK(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    CHECK(view.byte_count == strlen(before) && memcmp(view.bytes, before, view.byte_count) == 0);
    if (expected != UMI_STATUS_OK)
        CHECK(edits == NULL);
    else
    {
        size_t applied = 0U;
        UmiEditorWorkspaceEditSnapshot snapshot;
        CHECK(umi_editor_workspace_edit_set_snapshot(edits, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.applicable && snapshot.finalized && snapshot.conflict_count == 0U);
        if (strcmp(name, "stale-apply") == 0)
        {
            CHECK(umi_editor_text_buffer_insert(buffer, 0U, "!", 1U) == UMI_STATUS_OK);
            CHECK(umi_editor_workspace_edit_set_apply_document(edits, context.document_uri, buffer, 1,
                                                               &applied) == UMI_STATUS_INVALID_STATE);
            CHECK(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
            CHECK(view.byte_count == strlen(before) + 1U && view.bytes[0] == '!');
            CHECK(memcmp(view.bytes + 1U, before, strlen(before)) == 0);
        }
        else
        {
            CHECK(umi_editor_workspace_edit_set_apply_document(edits, context.document_uri, buffer, 1,
                                                               &applied) == UMI_STATUS_OK);
            CHECK(applied == snapshot.edit_count);
            CHECK(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
            CHECK(view.byte_count == strlen(after) && memcmp(view.bytes, after, view.byte_count) == 0);
        }
    }
    umi_cancellation_token_destroy(cancel);
    umi_editor_workspace_edit_set_destroy(edits);
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
