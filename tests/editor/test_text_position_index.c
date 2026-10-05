/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_position_index.c
 * PURPOSE: Check owned indexed coordinates across Unicode, line endings and sparse boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_position_index.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        if (!(condition))                                                                                    \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
typedef struct PositionCase
{
    const char *name, *source;
    size_t offset;
    uint64_t line, column;
    UmiStatus expected;
} PositionCase;
static const PositionCase cases[] = {
    {"empty", "", 0U, 0U, 0U, UMI_STATUS_OK},
    {"ascii", "abc", 2U, 0U, 2U, UMI_STATUS_OK},
    {"end", "abc", 3U, 0U, 3U, UMI_STATUS_OK},
    {"lf", "ab\ncd", 4U, 1U, 1U, UMI_STATUS_OK},
    {"crlf", "ab\r\ncd", 5U, 1U, 1U, UMI_STATUS_OK},
    {"cr", "ab\rcd", 4U, 1U, 1U, UMI_STATUS_OK},
    {"trailing-lf", "ab\n", 3U, 1U, 0U, UMI_STATUS_OK},
    {"trailing-cr", "ab\r", 3U, 1U, 0U, UMI_STATUS_OK},
    {"trailing-crlf", "ab\r\n", 4U, 1U, 0U, UMI_STATUS_OK},
    {"mixed", "a\r\nb\rc\nd", 7U, 3U, 0U, UMI_STATUS_OK},
    {"empty-line", "\n\nx", 1U, 1U, 0U, UMI_STATUS_OK},
    {"accent", "caf\xc3\xa9", 5U, 0U, 4U, UMI_STATUS_OK},
    {"combining", "e\xcc\x81", 3U, 0U, 2U, UMI_STATUS_OK},
    {"supplementary",
     "a\xf0\x9f\x98\x80"
     "b",
     5U, 0U, 3U, UMI_STATUS_OK},
    {"cjk", "\xe6\x96\x87x", 3U, 0U, 1U, UMI_STATUS_OK},
    {"invalid-leading", "\xff", 1U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"invalid-continuation", "\xc3x", 2U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"overlong", "\xe0\x80\x80", 3U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"encoded-surrogate", "\xed\xa0\x80", 3U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"out-of-unicode", "\xf4\x90\x80\x80", 4U, 0U, 2U, UMI_STATUS_PARSE_ERROR},
    {"truncated", "\xf0\x9f", 2U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"invalid-prefix", "\xff\nx", 3U, 1U, 1U, UMI_STATUS_PARSE_ERROR}};

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    size_t choice;
    for (choice = 0U; choice < sizeof(cases) / sizeof(cases[0]); ++choice)
        if (strcmp(mode, cases[choice].name) == 0)
            break;
    char *owned = NULL;
    const char *text = "abc";
    size_t bytes = 3U;
    if (choice < sizeof(cases) / sizeof(cases[0]))
    {
        text = cases[choice].source;
        bytes = strlen(text);
    }
    else if (strcmp(mode, "ownership") == 0)
    {
        owned = malloc(4U);
        CHECK(owned != NULL);
        memcpy(owned, "abc", 4U);
        text = owned;
    }
    else if (strcmp(mode, "many-lines") == 0)
    {
        bytes = 30000U;
        owned = malloc(bytes);
        CHECK(owned != NULL);
        for (size_t i = 0U; i < bytes; i += 3U)
            memcpy(owned + i, "x\r\n", 3U);
        text = owned;
    }
    else if (strcmp(mode, "checkpoint-unicode") == 0 || strcmp(mode, "checkpoint-crlf") == 0)
    {
        owned = malloc(140U);
        CHECK(owned != NULL);
        memset(owned, 'a', 127U);
        if (strcmp(mode, "checkpoint-unicode") == 0)
        {
            memcpy(owned + 127U, "\xf0\x9f\x8c\x8dZ", 5U);
            bytes = 132U;
        }
        else
        {
            memcpy(owned + 127U, "\r\nZ", 3U);
            bytes = 130U;
        }
        text = owned;
    }
    else if (strcmp(mode, "embedded-nul") == 0)
    {
        text = "a\0b";
        bytes = 3U;
    }
    else if (strcmp(mode, "empty-null") == 0)
    {
        text = NULL;
        bytes = 0U;
    }
    else if (strcmp(mode, "limit") == 0)
    {
        bytes = 16U * 1024U * 1024U + 1U;
        /* Oversize metadata is refused before the source pointer is read. */
        text = "x";
    }
    else if (strcmp(mode, "inside-utf8") == 0 || strcmp(mode, "inside-surrogate") == 0)
    {
        text = "\xf0\x9f\x8c\x8d";
        bytes = 4U;
    }
    else if (strcmp(mode, "inside-crlf") == 0)
    {
        text = "a\r\nb";
        bytes = 4U;
    }
    else if (strcmp(mode, "invalid-suffix") == 0)
    {
        text = "ok\xff";
        bytes = 3U;
    }
    else
        CHECK(strcmp(mode, "cancelled") == 0 || strcmp(mode, "past-line") == 0 ||
              strcmp(mode, "past-column") == 0 || strcmp(mode, "past-byte") == 0 ||
              strcmp(mode, "invalid-view") == 0 || strcmp(mode, "arguments") == 0);
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.revision = 19U;
    view.save_revision = 12U;
    view.dirty = 1;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    if (strcmp(mode, "invalid-view") == 0)
        view.capacity = 1U;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
        umi_cancellation_token_request(cancel);
    UmiEditorTextPositionIndex *index = NULL;
    UmiStatus status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
    UmiStatus expected = choice < sizeof(cases) / sizeof(cases[0]) ? cases[choice].expected : UMI_STATUS_OK;
    if (strcmp(mode, "limit") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "cancelled") == 0)
        expected = UMI_STATUS_CANCELLED;
    if (strcmp(mode, "invalid-view") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "invalid-suffix") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    CHECK(status == expected);
    if (status != UMI_STATUS_OK)
    {
        CHECK(index == NULL);
        goto done;
    }
    if (strcmp(mode, "ownership") == 0)
    {
        memset(owned, 'z', 3U);
        free(owned);
        owned = NULL;
    }
    UmiEditorTextBufferView captured;
    CHECK(UmiEditorTextPositionIndexView(index, &captured) == UMI_STATUS_OK && captured.byte_count == bytes);
    CHECK(captured.revision == 19U && captured.save_revision == 12U && captured.dirty == 1);
    if (strcmp(mode, "ownership") == 0)
        CHECK(memcmp(captured.bytes, "abc", 3U) == 0);
    UmiEditorTextPosition position = {99U, 98U};
    size_t offset = 97U;
    if (choice < sizeof(cases) / sizeof(cases[0]))
    {
        const PositionCase *row = &cases[choice];
        CHECK(UmiEditorTextPositionIndexAt(index, row->offset, &position) == UMI_STATUS_OK);
        CHECK(position.line == row->line && position.utf16_column == row->column);
        CHECK(UmiEditorTextPositionIndexResolve(index, position, &offset) == UMI_STATUS_OK &&
              offset == row->offset);
    }
    else if (strcmp(mode, "many-lines") == 0)
    {
        for (size_t line = 0U; line < 10000U; line += 97U)
        {
            CHECK(UmiEditorTextPositionIndexResolve(index, (UmiEditorTextPosition){line, 1U}, &offset) ==
                  UMI_STATUS_OK);
            CHECK(offset == line * 3U + 1U);
            CHECK(UmiEditorTextPositionIndexAt(index, offset, &position) == UMI_STATUS_OK);
            CHECK(position.line == line && position.utf16_column == 1U);
        }
        CHECK(UmiEditorTextPositionIndexAt(index, 30000U, &position) == UMI_STATUS_OK);
        CHECK(position.line == 10000U && position.utf16_column == 0U);
    }
    else if (strcmp(mode, "checkpoint-unicode") == 0)
    {
        CHECK(UmiEditorTextPositionIndexAt(index, 131U, &position) == UMI_STATUS_OK);
        CHECK(position.line == 0U && position.utf16_column == 129U);
        CHECK(UmiEditorTextPositionIndexResolve(index, (UmiEditorTextPosition){0U, 128U}, &offset) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              offset == 97U);
    }
    else if (strcmp(mode, "checkpoint-crlf") == 0)
    {
        CHECK(UmiEditorTextPositionIndexAt(index, 129U, &position) == UMI_STATUS_OK);
        CHECK(position.line == 1U && position.utf16_column == 0U);
        CHECK(UmiEditorTextPositionIndexAt(index, 128U, &position) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(position.line == 1U && position.utf16_column == 0U);
    }
    else if (strcmp(mode, "inside-utf8") == 0 || strcmp(mode, "inside-crlf") == 0 ||
             strcmp(mode, "past-byte") == 0)
    {
        size_t invalid = strcmp(mode, "inside-utf8") == 0 ? 1U : strcmp(mode, "inside-crlf") == 0 ? 2U : 4U;
        CHECK(UmiEditorTextPositionIndexAt(index, invalid, &position) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(position.line == 99U && position.utf16_column == 98U);
    }
    else if (strcmp(mode, "inside-surrogate") == 0 || strcmp(mode, "past-line") == 0 ||
             strcmp(mode, "past-column") == 0)
    {
        UmiEditorTextPosition invalid = strcmp(mode, "inside-surrogate") == 0
                                            ? (UmiEditorTextPosition){0U, 1U}
                                        : strcmp(mode, "past-line") == 0 ? (UmiEditorTextPosition){1U, 0U}
                                                                         : (UmiEditorTextPosition){0U, 4U};
        CHECK(UmiEditorTextPositionIndexResolve(index, invalid, &offset) == UMI_STATUS_INVALID_ARGUMENT &&
              offset == 97U);
    }
    else if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiEditorTextPositionIndexAt(NULL, 0U, &position) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEditorTextPositionIndexResolve(index, (UmiEditorTextPosition){0}, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEditorTextPositionIndexView(NULL, &captured) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        CHECK(UmiEditorTextPositionIndexAt(index, bytes, &position) == UMI_STATUS_OK);
        CHECK(position.line == 0U && position.utf16_column == bytes);
    }
done:
    UmiEditorTextPositionIndexDestroy(index);
    umi_cancellation_token_destroy(cancel);
    free(owned);
    return 0;
}
