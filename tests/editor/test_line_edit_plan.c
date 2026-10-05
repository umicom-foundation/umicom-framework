/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_line_edit_plan.c
 * PURPOSE: Check reused line transforms, exact source boundaries, selection policy and immutable proposal ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/line_edit_plan.h"
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
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"delete-middle",
                           "delete-last",
                           "delete-only",
                           "duplicate-middle",
                           "duplicate-last",
                           "duplicate-crlf",
                           "duplicate-empty",
                           "move-up",
                           "move-down",
                           "move-unequal",
                           "move-unicode",
                           "move-top",
                           "move-bottom",
                           "join",
                           "join-empty",
                           "join-crlf-empty",
                           "indent",
                           "indent-selection",
                           "indent-reverse",
                           "indent-tab",
                           "outdent",
                           "outdent-tab",
                           "comment",
                           "uncomment",
                           "comment-mixed",
                           "comment-token",
                           "comment-blank",
                           "trim",
                           "trim-unchanged",
                           "owned",
                           "invalid-utf8",
                           "split-scalar",
                           "split-crlf",
                           "bare-cr",
                           "embedded-null",
                           "invalid-kind",
                           "invalid-indent",
                           "invalid-comment",
                           "oversized-indent",
                           "oversized-comment",
                           "null-token",
                           "invalid-cursor",
                           "invalid-selection",
                           "cancelled",
                           "null-output",
                           "capacity"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    char source[128] = "a\nbb\nccc";
    const char *expected = "a\nccc";
    size_t expected_cursor = 2U, selected = 0U;
    UmiEditorEditCommandRequest request;
    CHECK(umi_editor_edit_command_request_initialize(&request, UMI_EDITOR_EDIT_COMMAND_DELETE_LINE, 2U) ==
          UMI_STATUS_OK);
    UmiStatus status = UMI_STATUS_OK;
    if (strcmp(mode, "delete-last") == 0)
    {
        request.cursor_offset = 5U;
        expected = "a\nbb";
        expected_cursor = 4U;
    }
    if (strcmp(mode, "delete-only") == 0)
    {
        strcpy(source, "abc");
        request.cursor_offset = 1U;
        expected = "";
        expected_cursor = 0U;
    }
    if (strncmp(mode, "duplicate-", 10U) == 0 || strcmp(mode, "owned") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE;
        expected = "a\nbb\nbb\nccc";
        expected_cursor = 5U;
        if (strcmp(mode, "duplicate-last") == 0)
        {
            request.cursor_offset = 6U;
            expected = "a\nbb\nccc\nccc";
            expected_cursor = 10U;
        }
        if (strcmp(mode, "duplicate-crlf") == 0)
        {
            strcpy(source, "a\r\nbb");
            request.cursor_offset = 4U;
            expected = "a\r\nbb\r\nbb";
            expected_cursor = 8U;
        }
        if (strcmp(mode, "duplicate-empty") == 0)
        {
            source[0] = '\0';
            request.cursor_offset = 0U;
            expected = "\n";
            expected_cursor = 1U;
        }
    }
    if (strncmp(mode, "move-", 5U) == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP;
        expected = "bb\na\nccc";
        expected_cursor = 0U;
        if (strcmp(mode, "move-down") == 0)
        {
            request.kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN;
            expected = "a\nccc\nbb";
            expected_cursor = 6U;
        }
        if (strcmp(mode, "move-unequal") == 0)
        {
            strcpy(source, "long\nx\n");
            request.kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN;
            request.cursor_offset = 3U;
            expected = "x\nlong\n";
            expected_cursor = 5U;
        }
        if (strcmp(mode, "move-unicode") == 0)
        {
            strcpy(source, "\xf0\x9f\x8c\x8d\nx\n");
            request.kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN;
            request.cursor_offset = 4U;
            expected = "x\n\xf0\x9f\x8c\x8d\n";
            expected_cursor = 6U;
        }
        if (strcmp(mode, "move-top") == 0)
        {
            request.cursor_offset = 0U;
            status = UMI_STATUS_NOT_FOUND;
        }
        if (strcmp(mode, "move-bottom") == 0)
        {
            request.kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN;
            request.cursor_offset = 5U;
            status = UMI_STATUS_NOT_FOUND;
        }
    }
    if (strcmp(mode, "join") == 0 || strcmp(mode, "join-empty") == 0 || strcmp(mode, "join-crlf-empty") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT;
        expected = "a\nbb ccc";
        expected_cursor = 5U;
        if (strcmp(mode, "join-crlf-empty") == 0)
        {
            strcpy(source, "a\r\n\r\nccc");
            request.cursor_offset = 0U;
            expected = "a\r\nccc";
            expected_cursor = 1U;
        }
        if (strcmp(mode, "join-empty") == 0)
        {
            strcpy(source, "a\n\nccc");
            expected = "a\nccc";
            expected_cursor = 2U;
        }
    }
    if (strncmp(mode, "indent", 6U) == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        expected = "a\n    bb\nccc";
        expected_cursor = 2U;
        if (strcmp(mode, "indent-tab") == 0)
        {
            request.indent_text = "\t";
            request.indent_byte_count = 1U;
            expected = "a\n\tbb\nccc";
        }
    }
    if (strcmp(mode, "outdent") == 0 || strcmp(mode, "outdent-tab") == 0)
    {
        strcpy(source, strcmp(mode, "outdent-tab") == 0 ? "a\n\tbb\nccc" : "a\n  bb\nccc");
        request.kind = UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES;
        expected = "a\nbb\nccc";
        expected_cursor = 2U;
    }
    if (strncmp(mode, "comment", 7U) == 0 || strcmp(mode, "uncomment") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
        expected = "a\n// bb\nccc";
        expected_cursor = 2U;
        if (strcmp(mode, "uncomment") == 0)
        {
            strcpy(source, "a\n// bb\nccc");
            expected = "a\nbb\nccc";
        }
        if (strcmp(mode, "comment-token") == 0)
        {
            request.line_comment = "#";
            request.line_comment_byte_count = 1U;
            expected = "a\n# bb\nccc";
        }
        if (strcmp(mode, "comment-blank") == 0)
        {
            strcpy(source, "a\n  \nccc");
            expected = "a\n  \nccc";
        }
    }
    if (strncmp(mode, "trim", 4U) == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE;
        strcpy(source, "a \r\nbb\t\r\nccc ");
        expected = "a\r\nbb\r\nccc";
        expected_cursor = 0U;
        if (strcmp(mode, "trim-unchanged") == 0)
        {
            strcpy(source, "a\nbb\nccc");
            expected = source;
            expected_cursor = 2U;
        }
    }
    request.selection_start = request.selection_end = request.cursor_offset;
    if (strcmp(mode, "indent-selection") == 0 || strcmp(mode, "indent-reverse") == 0)
    {
        request.selection_start = strcmp(mode, "indent-reverse") == 0 ? 5U : 0U;
        request.selection_end = strcmp(mode, "indent-reverse") == 0 ? 0U : 5U;
        expected = "    a\n    bb\nccc";
        expected_cursor = 0U;
        selected = 13U;
    }
    if (strcmp(mode, "comment-mixed") == 0)
    {
        strcpy(source, "a\n// bb\nccc");
        request.selection_start = 0U;
        request.selection_end = 8U;
        expected = "// a\n// // bb\nccc";
        expected_cursor = 0U;
        selected = 14U;
    }
    size_t bytes = strlen(source);
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        strcpy(source, "a\xc0\x80");
        bytes = 3U;
        status = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "split-scalar") == 0)
    {
        strcpy(source, "a\xf0\x9f\x8c\x8d");
        bytes = 5U;
        request.cursor_offset = 2U;
        request.selection_start = request.selection_end = 0U;
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "split-crlf") == 0)
    {
        strcpy(source, "a\r\nb");
        bytes = 4U;
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "bare-cr") == 0)
    {
        strcpy(source, "a\rbb");
        bytes = 4U;
        status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "embedded-null") == 0)
    {
        source[3] = '\0';
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-kind") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT;
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-indent") == 0 || strcmp(mode, "oversized-indent") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        request.indent_text = strcmp(mode, "invalid-indent") == 0 ? "x" : "                 ";
        request.indent_byte_count = strlen(request.indent_text);
        status =
            strcmp(mode, "invalid-indent") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "invalid-comment") == 0 || strcmp(mode, "oversized-comment") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
        request.line_comment =
            strcmp(mode, "invalid-comment") == 0 ? "//\n" : "/////////////////////////////////";
        request.line_comment_byte_count = strlen(request.line_comment);
        status =
            strcmp(mode, "invalid-comment") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "null-token") == 0)
    {
        request.kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        request.indent_text = NULL;
        request.indent_byte_count = 1U;
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-cursor") == 0)
    {
        request.cursor_offset = bytes + 1U;
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-selection") == 0)
    {
        request.selection_end = bytes + 1U;
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "capacity") == 0)
    {
        bytes = 8U * 1024U * 1024U + 1U;
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        status = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "null-output") == 0)
        status = UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorLineEditPlan *plan = NULL;
    CHECK(UmiEditorLineEditPlanCreate(source, bytes, &request, cancel,
                                      strcmp(mode, "null-output") == 0 ? NULL : &plan) == status);
    if (status == UMI_STATUS_OK)
    {
        if (strcmp(mode, "owned") == 0)
            memset(source, 0, sizeof(source));
        UmiEditorLineEditSummary summary;
        const char *text = NULL;
        size_t size = 0U;
        CHECK(UmiEditorLineEditPlanInspect(plan, &summary) == UMI_STATUS_OK &&
              UmiEditorLineEditPlanRead(plan, &text, &size) == UMI_STATUS_OK);
        CHECK(size == strlen(expected) && strcmp(text, expected) == 0);
        CHECK(summary.source_bytes == bytes && summary.proposed_bytes == size &&
              summary.cursor_offset == expected_cursor && summary.selection_bytes == selected);
        CHECK(summary.changed == (strcmp(mode, "comment-blank") != 0 && strcmp(mode, "trim-unchanged") != 0));
    }
    else
        CHECK(plan == NULL);
    UmiEditorLineEditPlanDestroy(plan);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
