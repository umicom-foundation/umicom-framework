/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_position.c
 * PURPOSE: Check Unicode boundaries and reversible positions across supported line endings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_position.h"
#include <stdio.h>
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
    const char *name = argv[1], *source = "abc";
    size_t index;
    for (index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index)
        if (strcmp(name, cases[index].name) == 0)
            break;
    if (index < sizeof(cases) / sizeof(cases[0]))
        source = cases[index].source;
    else if (strcmp(name, "inside-utf8") == 0 || strcmp(name, "inside-surrogate") == 0)
        source = "\xf0\x9f\x98\x80";
    else if (strcmp(name, "inside-crlf") == 0)
        source = "a\r\nb";
    UmiEditorTextBuffer *buffer = NULL;
    CHECK(umi_editor_text_buffer_create(64U, &buffer) == UMI_STATUS_OK);
    CHECK(umi_editor_text_buffer_set(buffer, source, strlen(source)) == UMI_STATUS_OK);
    UmiEditorTextBufferView view;
    CHECK(umi_editor_text_buffer_view(buffer, &view) == UMI_STATUS_OK);
    UmiEditorTextPosition position = {99U, 98U};
    size_t offset = 97U;
    if (index < sizeof(cases) / sizeof(cases[0]))
    {
        const PositionCase *row = &cases[index];
        CHECK(UmiEditorTextViewPositionAt(&view, row->offset, &position) == row->expected);
        CHECK(UmiEditorTextViewResolvePosition(&view, (UmiEditorTextPosition){row->line, row->column},
                                               &offset) == row->expected);
        if (row->expected == UMI_STATUS_OK)
            CHECK(position.line == row->line && position.utf16_column == row->column &&
                  offset == row->offset);
        else
            CHECK(position.line == 99U && position.utf16_column == 98U && offset == 97U);
    }
    else if (strcmp(name, "inside-utf8") == 0 || strcmp(name, "inside-crlf") == 0)
    {
        CHECK(UmiEditorTextViewPositionAt(&view, strcmp(name, "inside-utf8") == 0 ? 1U : 2U, &position) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(position.line == 99U && position.utf16_column == 98U);
    }
    else if (strcmp(name, "inside-surrogate") == 0 || strcmp(name, "past-column") == 0 ||
             strcmp(name, "past-line") == 0)
    {
        UmiEditorTextPosition invalid = {0U, strcmp(name, "inside-surrogate") == 0 ? 1U : 4U};
        if (strcmp(name, "past-line") == 0)
        {
            invalid.line = 1U;
            invalid.utf16_column = 0U;
        }
        CHECK(UmiEditorTextViewResolvePosition(&view, invalid, &offset) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(offset == 97U);
    }
    else if (strcmp(name, "past-byte") == 0)
    {
        CHECK(UmiEditorTextViewPositionAt(&view, view.byte_count + 1U, &position) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(position.line == 99U && position.utf16_column == 98U);
    }
    else if (strcmp(name, "arguments") == 0)
    {
        CHECK(UmiEditorTextViewResolvePosition(NULL, (UmiEditorTextPosition){0U, 0U}, &offset) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiEditorTextViewPositionAt(&view, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        view.struct_size = 0U;
        CHECK(UmiEditorTextViewPositionAt(&view, 0U, &position) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
        CHECK(0);
    umi_editor_text_buffer_destroy(buffer);
    return 0;
}
