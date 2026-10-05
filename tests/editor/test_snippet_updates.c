/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_snippet_updates.c
 * PURPOSE: Check atomic linked-placeholder edits, deterministic empty stops and preserved sessions after failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/snippet_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #c);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static void Template(UmiEditorSnippetTemplate *snippet, const char *body)
{
    memset(snippet, 0, sizeof(*snippet));
    snippet->struct_size = (uint32_t)sizeof(*snippet);
    snippet->api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    strcpy(snippet->id, "linked-template");
    strcpy(snippet->language_id, "c");
    strcpy(snippet->name, "Linked values");
    strcpy(snippet->body, body);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"grow",
                           "shrink",
                           "empty",
                           "same",
                           "unicode",
                           "multiline",
                           "literal-syntax",
                           "choice-value",
                           "later-ordinal",
                           "final-active",
                           "traversal",
                           "repeated-update",
                           "coincident",
                           "zero-before",
                           "adjacent",
                           "mirrors-after",
                           "base-offset",
                           "borrowed-input",
                           "value-limit",
                           "value-over",
                           "expanded-over",
                           "offset-over",
                           "unknown",
                           "final-stop",
                           "stale",
                           "cancelled",
                           "cancel-reset",
                           "embedded-nul",
                           "invalid-utf8",
                           "cancelled-session",
                           "completed-session",
                           "idle-session",
                           "null-session",
                           "null-text",
                           "read-invalid",
                           "restart",
                           "restart-cancel",
                           "restart-stale",
                           "restart-invalid",
                           "restart-invalid-text",
                           "restart-offset",
                           "restart-completed"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    UmiEditorSnippetSession *session = NULL;
    CHECK(umi_editor_snippet_session_create(&session) == UMI_STATUS_OK);
    UmiEditorSnippetTemplate *snippet = malloc(sizeof(*snippet));
    CHECK(snippet != NULL);
    const char *body = "before ${1:old} / $1 / ${2:end}$0 after";
    uint64_t base = strcmp(mode, "base-offset") == 0 ? 1000U : 0U;
    if (strcmp(mode, "coincident") == 0)
        body = "$1$2$0";
    if (strcmp(mode, "zero-before") == 0)
        body = "$2$1$0";
    if (strcmp(mode, "adjacent") == 0)
        body = "${1:a}${1:b}$0";
    if (strcmp(mode, "mirrors-after") == 0)
        body = "${2:end}-${1:old}-$2-$1$0";
    if (strcmp(mode, "choice-value") == 0)
        body = "${1|red,green,blue|}-$1$0";
    if (strcmp(mode, "offset-over") == 0)
    {
        body = "$1$0";
        base = UINT64_MAX - 1U;
    }
    Template(snippet, body);
    if (strcmp(mode, "expanded-over") == 0)
    {
        snippet->body[0] = '\0';
        for (size_t i = 0U; i < 40U; ++i)
            strcat(snippet->body, "${1:x}");
        strcat(snippet->body, "$0");
    }
    if (strcmp(mode, "idle-session") != 0)
        CHECK(umi_editor_snippet_session_start(session, snippet, base) == UMI_STATUS_OK);
    if (strcmp(mode, "final-active") == 0)
        CHECK(umi_editor_snippet_session_select(session, 0U) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled-session") == 0)
        CHECK(umi_editor_snippet_session_cancel(session) == UMI_STATUS_OK);
    if (strcmp(mode, "completed-session") == 0 || strcmp(mode, "restart-completed") == 0)
    {
        CHECK(umi_editor_snippet_session_select(session, 0U) == UMI_STATUS_OK);
        CHECK(umi_editor_snippet_session_next(session) == UMI_STATUS_OK);
    }
    UmiEditorSnippetSessionSnapshot before;
    CHECK(umi_editor_snippet_session_snapshot(session, &before) == UMI_STATUS_OK);
    char *original = malloc(UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY);
    CHECK(original != NULL);
    CHECK(umi_editor_snippet_session_expanded_text(session, original, UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY) ==
          UMI_STATUS_OK);
    size_t count = umi_editor_snippet_session_placeholder_count(session);
    UmiEditorSnippetPlaceholder *old = count == 0U ? NULL : calloc(count, sizeof(*old));
    CHECK(count == 0U || old != NULL);
    for (size_t i = 0U; i < count; ++i)
        CHECK(umi_editor_snippet_session_placeholder_at(session, i, &old[i]) == UMI_STATUS_OK);
    const char *replacement = "replacement";
    size_t bytes = strlen(replacement);
    uint32_t ordinal = 1U;
    uint64_t revision = before.revision;
    const char *expected = "before replacement / replacement / end after";
    UmiStatus wanted = UMI_STATUS_OK;
    char large[513];
    memset(large, 'x', sizeof(large));
    large[512] = '\0';
    if (strcmp(mode, "shrink") == 0)
    {
        replacement = "x";
        expected = "before x / x / end after";
    }
    if (strcmp(mode, "empty") == 0)
    {
        replacement = "";
        expected = "before  /  / end after";
    }
    if (strcmp(mode, "same") == 0)
    {
        replacement = "old";
        expected = original;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        replacement = "\xf0\x9f\x98\x80";
        expected = "before \xf0\x9f\x98\x80 / \xf0\x9f\x98\x80 / end after";
    }
    if (strcmp(mode, "multiline") == 0)
    {
        replacement = "a\r\nb";
        expected = "before a\r\nb / a\r\nb / end after";
    }
    if (strcmp(mode, "literal-syntax") == 0)
    {
        replacement = "${2:literal}";
        expected = "before ${2:literal} / ${2:literal} / end after";
    }
    if (strcmp(mode, "choice-value") == 0)
    {
        replacement = "custom";
        expected = "custom-custom";
    }
    if (strcmp(mode, "later-ordinal") == 0)
    {
        ordinal = 2U;
        expected = "before old / old / replacement after";
    }
    if (strcmp(mode, "coincident") == 0 || strcmp(mode, "zero-before") == 0)
        expected = "replacement";
    if (strcmp(mode, "adjacent") == 0)
        expected = "replacementreplacement";
    if (strcmp(mode, "mirrors-after") == 0)
        expected = "end-replacement-end-replacement";
    bytes = strlen(replacement);
    if (strcmp(mode, "borrowed-input") == 0)
    {
        const char *borrowed = NULL;
        size_t length = 0U;
        CHECK(UmiEditorSnippetSessionRead(session, &borrowed, &length) == UMI_STATUS_OK);
        replacement = borrowed + 7U;
        bytes = 3U;
        expected = original;
    }
    if (strcmp(mode, "value-limit") == 0)
    {
        replacement = large;
        bytes = 511U;
    }
    if (strcmp(mode, "value-over") == 0)
    {
        replacement = large;
        bytes = 512U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "expanded-over") == 0)
    {
        replacement = large;
        bytes = 511U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "offset-over") == 0)
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "unknown") == 0)
    {
        ordinal = 999U;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "final-stop") == 0)
    {
        ordinal = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "stale") == 0)
    {
        revision = before.revision - 1U;
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "embedded-nul") == 0)
    {
        replacement = "a\0b";
        bytes = 3U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        replacement = "\xf0\x9f";
        bytes = 2U;
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "cancelled-session") == 0 || strcmp(mode, "completed-session") == 0 ||
        strcmp(mode, "idle-session") == 0)
        wanted = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "null-text") == 0)
    {
        replacement = NULL;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "null-session") == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0 || strcmp(mode, "cancel-reset") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    UmiStatus status;
    int restart = strncmp(mode, "restart", 7U) == 0;
    if (restart)
    {
        Template(snippet, "${3:new}-$3$0");
        base = 42U;
        expected = "new-new";
        if (strcmp(mode, "restart-cancel") == 0)
        {
            umi_cancellation_token_request(cancel);
            wanted = UMI_STATUS_CANCELLED;
        }
        if (strcmp(mode, "restart-stale") == 0)
        {
            revision = before.revision - 1U;
            wanted = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "restart-invalid") == 0)
        {
            snippet->language_id[0] = '\0';
            wanted = UMI_STATUS_INVALID_ARGUMENT;
        }
        if (strcmp(mode, "restart-invalid-text") == 0)
        {
            strcpy(snippet->body, "\xc0\x80");
            wanted = UMI_STATUS_PARSE_ERROR;
        }
        if (strcmp(mode, "restart-offset") == 0)
        {
            base = UINT64_MAX;
            wanted = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = UmiEditorSnippetSessionRestart(session, revision, snippet, base, cancel);
    }
    else if (strcmp(mode, "read-invalid") == 0)
    {
        const char *sentinel = "kept";
        size_t length = 42U;
        CHECK(UmiEditorSnippetSessionRead(NULL, &sentinel, &length) == UMI_STATUS_INVALID_ARGUMENT &&
              strcmp(sentinel, "kept") == 0 && length == 42U);
        CHECK(UmiEditorSnippetSessionRead(session, NULL, &length) == UMI_STATUS_INVALID_ARGUMENT &&
              length == 42U);
        CHECK(UmiEditorSnippetSessionRead(session, &sentinel, NULL) == UMI_STATUS_INVALID_ARGUMENT &&
              strcmp(sentinel, "kept") == 0);
        wanted = UMI_STATUS_INVALID_ARGUMENT;
        status = wanted;
    }
    else
        status = UmiEditorSnippetSessionReplace(strcmp(mode, "null-session") == 0 ? NULL : session, revision,
                                                ordinal, replacement, bytes, cancel);
    CHECK(status == wanted);
    UmiEditorSnippetSessionSnapshot after;
    CHECK(umi_editor_snippet_session_snapshot(session, &after) == UMI_STATUS_OK);
    const char *text = NULL;
    size_t length = 0U;
    CHECK(UmiEditorSnippetSessionRead(session, &text, &length) == UMI_STATUS_OK);
    if (status == UMI_STATUS_OK)
    {
        CHECK(after.revision == before.revision + 1U);
        if (strcmp(mode, "value-limit") == 0)
            CHECK(length == strlen(original) + 2U * (511U - 3U));
        else
            CHECK(length == strlen(expected) && strcmp(text, expected) == 0);
        if (!restart)
        {
            CHECK(after.active_ordinal == before.active_ordinal &&
                  after.placeholder_count == before.placeholder_count);
            uint64_t last = base;
            for (size_t i = 0U; i < count; ++i)
            {
                UmiEditorSnippetPlaceholder value;
                CHECK(umi_editor_snippet_session_placeholder_at(session, i, &value) == UMI_STATUS_OK);
                CHECK(value.start_byte_offset >= last && value.end_byte_offset >= value.start_byte_offset &&
                      value.end_byte_offset - base <= length);
                CHECK(value.ordinal == old[i].ordinal && value.primary == old[i].primary &&
                      value.final_stop == old[i].final_stop);
                if (value.ordinal == ordinal)
                    CHECK(value.end_byte_offset - value.start_byte_offset == bytes &&
                          strlen(value.default_text) == bytes);
                last = value.end_byte_offset;
            }
            if (strcmp(mode, "coincident") == 0 || strcmp(mode, "zero-before") == 0)
            {
                UmiEditorSnippetPlaceholder other;
                CHECK(umi_editor_snippet_session_placeholder_at(
                          session, strcmp(mode, "zero-before") == 0 ? 0U : 1U, &other) == UMI_STATUS_OK);
                CHECK(other.start_byte_offset ==
                      (strcmp(mode, "zero-before") == 0 ? 0U : strlen(replacement)));
            }
        }
        else
            CHECK(after.active_ordinal == 3U && after.insertion_byte_offset == 42U &&
                  after.state == UMI_EDITOR_SNIPPET_ACTIVE);
    }
    else
    {
        CHECK(after.revision == before.revision && after.state == before.state &&
              after.active_ordinal == before.active_ordinal && strcmp(text, original) == 0);
        CHECK(after.placeholder_count == count);
        for (size_t i = 0U; i < count; ++i)
        {
            UmiEditorSnippetPlaceholder value;
            CHECK(umi_editor_snippet_session_placeholder_at(session, i, &value) == UMI_STATUS_OK);
            CHECK(memcmp(&value, &old[i], sizeof(value)) == 0);
        }
    }
    if (strcmp(mode, "cancel-reset") == 0)
    {
        umi_cancellation_token_destroy(cancel);
        cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        CHECK(UmiEditorSnippetSessionReplace(session, before.revision, 1U, "again", 5U, cancel) ==
              UMI_STATUS_OK);
    }
    if (strcmp(mode, "repeated-update") == 0)
    {
        CHECK(UmiEditorSnippetSessionReplace(session, after.revision, 1U, "x", 1U, NULL) == UMI_STATUS_OK);
        CHECK(UmiEditorSnippetSessionRead(session, &text, &length) == UMI_STATUS_OK &&
              strcmp(text, "before x / x / end after") == 0);
    }
    if (strcmp(mode, "traversal") == 0)
    {
        CHECK(umi_editor_snippet_session_next(session) == UMI_STATUS_OK);
        UmiEditorSnippetPlaceholder active;
        CHECK(umi_editor_snippet_session_active(session, &active) == UMI_STATUS_OK && active.ordinal == 2U);
        CHECK(umi_editor_snippet_session_next(session) == UMI_STATUS_OK);
        CHECK(umi_editor_snippet_session_active(session, &active) == UMI_STATUS_OK && active.ordinal == 0U);
        CHECK(umi_editor_snippet_session_previous(session) == UMI_STATUS_OK);
        CHECK(umi_editor_snippet_session_active(session, &active) == UMI_STATUS_OK && active.ordinal == 2U);
    }
    umi_cancellation_token_destroy(cancel);
    free(original);
    free(old);
    free(snippet);
    umi_editor_snippet_session_destroy(session);
    return 0;
}
