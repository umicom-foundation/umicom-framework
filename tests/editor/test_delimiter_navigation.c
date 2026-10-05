/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_delimiter_navigation.c
 * PURPOSE: Exercise lexical bracket discovery, original UTF-8 offsets and bounded failure without product processes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/delimiter_navigation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Case
{
    const char *name, *text;
    size_t caret;
    UmiEditorDelimiterSyntax syntax;
    int enclosing;
    size_t first, last, anchor;
    UmiStatus status;
} Case;
static UmiEditorTextBufferView View(const char *text, size_t bytes)
{
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    return view;
}
static const Case cases[] = {
    {"c-spliced-cr", "(// }\\\r ]\rx)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 11U, 0U, UMI_STATUS_OK},
    {"c-adjacent-character", "(foo1']')", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 8U, 0U, UMI_STATUS_OK},
    {"c-prefixed-string", "(u8\"}\")", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 6U, 0U, UMI_STATUS_OK},
    {"round-open", "(x)", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 2U, 0U, UMI_STATUS_OK},
    {"round-close", "(x)", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 2U, 2U, UMI_STATUS_OK},
    {"after-close", "(x)", 3U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 2U, 2U, UMI_STATUS_OK},
    {"after-open", "(x)", 1U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 2U, 0U, UMI_STATUS_OK},
    {"square", "[x]", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 2U, 0U, UMI_STATUS_OK},
    {"brace", "{x}", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 2U, 2U, UMI_STATUS_OK},
    {"adjacent", "()[]", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 2U, 3U, 2U, UMI_STATUS_OK},
    {"adjacent-unfinished", "()[", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX,
     UMI_STATUS_NOT_FOUND},
    {"nested-match", "([{}])", 1U, UMI_EDITOR_DELIMITER_LITERAL, 0, 1U, 4U, 1U, UMI_STATUS_OK},
    {"inner-enclosing", "([word])", 3U, UMI_EDITOR_DELIMITER_LITERAL, 1, 1U, 6U, SIZE_MAX, UMI_STATUS_OK},
    {"outer-enclosing", "( [word] )", 8U, UMI_EDITOR_DELIMITER_LITERAL, 1, 0U, 9U, SIZE_MAX, UMI_STATUS_OK},
    {"empty-enclosing", "{}", 1U, UMI_EDITOR_DELIMITER_LITERAL, 1, 0U, 1U, SIZE_MAX, UMI_STATUS_OK},
    {"no-enclosing", "(x) text", 4U, UMI_EDITOR_DELIMITER_LITERAL, 1, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"end-after-enclosing", "(x)", 3U, UMI_EDITOR_DELIMITER_LITERAL, 1, 0U, 0U, SIZE_MAX,
     UMI_STATUS_NOT_FOUND},
    {"crossed", "([)]", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"stray-close", ") (x)", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 2U, 4U, 2U, UMI_STATUS_OK},
    {"recover-crossed", "([)]{x}", 4U, UMI_EDITOR_DELIMITER_LITERAL, 0, 4U, 6U, 4U, UMI_STATUS_OK},
    {"unfinished-parent", "( [x]", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 2U, 4U, 2U, UMI_STATUS_OK},
    {"unfinished-open", "(", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"stray-only", ")", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"no-bracket", "words", 3U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"empty", "", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"literal-quote", "\"(x)\"", 1U, UMI_EDITOR_DELIMITER_LITERAL, 0, 1U, 3U, 1U, UMI_STATUS_OK},
    {"c-string", "(\"}\")", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 4U, 0U, UMI_STATUS_OK},
    {"c-character", "(']')", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 4U, 0U, UMI_STATUS_OK},
    {"c-escaped-quote", "(\"\\\" }\")", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 7U, 0U, UMI_STATUS_OK},
    {"c-string-hidden", "\"(x)\"", 1U, UMI_EDITOR_DELIMITER_C, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"c-line-comment", "( // }\n x)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 9U, 0U, UMI_STATUS_OK},
    {"c-block-comment", "(/* } ] */x)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 11U, 0U, UMI_STATUS_OK},
    {"c-comment-hidden", "/*(x)*/", 2U, UMI_EDITOR_DELIMITER_C, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"c-unclosed-string", "(\" )", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"c-unclosed-comment", "(/* )", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"c-number-separator", "(1'000)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 6U, 0U, UMI_STATUS_OK},
    {"c-hex-separator", "(0xA'B)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 6U, 0U, UMI_STATUS_OK},
    {"json-string", "{\"key\":\"}\"}", 0U, UMI_EDITOR_DELIMITER_JSON, 0, 0U, 10U, 0U, UMI_STATUS_OK},
    {"json-hidden", "\"[x]\"", 1U, UMI_EDITOR_DELIMITER_JSON, 0, 0U, 0U, SIZE_MAX, UMI_STATUS_NOT_FOUND},
    {"json-no-comments", "/*(x)*/", 2U, UMI_EDITOR_DELIMITER_JSON, 0, 2U, 4U, 2U, UMI_STATUS_OK},
    {"unicode", "雪(😀)", 3U, UMI_EDITOR_DELIMITER_LITERAL, 0, 3U, 8U, 3U, UMI_STATUS_OK},
    {"unicode-enclosing", "(雪😀)", 4U, UMI_EDITOR_DELIMITER_LITERAL, 1, 0U, 8U, SIZE_MAX, UMI_STATUS_OK},
    {"crlf", "(\r\nx)", 0U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 4U, 0U, UMI_STATUS_OK},
    {"crlf-interior", "(\r\nx)", 2U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX,
     UMI_STATUS_INVALID_ARGUMENT},
    {"utf8-interior", "雪(x)", 1U, UMI_EDITOR_DELIMITER_LITERAL, 0, 0U, 0U, SIZE_MAX,
     UMI_STATUS_INVALID_ARGUMENT},
    {"c-spliced-comment", "/\\\n* } */(x)", 9U, UMI_EDITOR_DELIMITER_C, 0, 9U, 11U, 9U, UMI_STATUS_OK},
    {"c-spliced-comment-end", "(/* } *\\\n/x)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 11U, 0U, UMI_STATUS_OK},
    {"c-spliced-line", "(// }\\\n ]\nx)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 11U, 0U, UMI_STATUS_OK},
    {"c-spliced-crlf", "(// }\\\r\n ]\r\nx)", 0U, UMI_EDITOR_DELIMITER_C, 0, 0U, 13U, 0U, UMI_STATUS_OK}};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiEditorDelimiterPair output, unchanged;
    memset(&output, 0x5a, sizeof(output));
    memcpy(&unchanged, &output, sizeof(output));
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i].name) == 0)
        {
            const Case *test = &cases[i];
            UmiEditorTextBufferView view = View(test->text, strlen(test->text));
            UmiStatus status = test->enclosing
                                   ? UmiEditorDelimiterEnclosing(&view, test->caret, test->syntax, &output)
                                   : UmiEditorDelimiterMatch(&view, test->caret, test->syntax, &output);
            CHECK(status == test->status);
            if (status == UMI_STATUS_OK)
            {
                CHECK(output.opening_offset == test->first && output.closing_offset == test->last &&
                      output.anchor_offset == test->anchor);
                CHECK(output.opening == test->text[test->first] && output.closing == test->text[test->last]);
            }
            else
                CHECK(memcmp(&output, &unchanged, sizeof(output)) == 0);
            return 0;
        }
    const char *special[] = {"null-view",    "null-output", "metadata",   "api",          "capacity",
                             "null-bytes",   "caret-over",  "syntax-low", "syntax-high",  "embedded-nul",
                             "invalid-utf8", "depth-limit", "depth-over", "source-limit", "source-over"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(special) / sizeof(special[0]); ++i)
        if (strcmp(mode, special[i]) == 0)
            known = 1;
    CHECK(known);
    UmiEditorTextBufferView view = View("()", 2U);
    const UmiEditorTextBufferView *input = &view;
    UmiEditorDelimiterPair *result = &output;
    UmiEditorDelimiterSyntax syntax = UMI_EDITOR_DELIMITER_LITERAL;
    size_t caret = 0U;
    char *allocated = NULL;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "null-view") == 0)
        input = NULL;
    if (strcmp(mode, "null-output") == 0)
        result = NULL;
    if (strcmp(mode, "metadata") == 0)
        view.struct_size = 0U;
    if (strcmp(mode, "api") == 0)
        view.api_version = 0U;
    if (strcmp(mode, "capacity") == 0)
        view.capacity = 1U;
    if (strcmp(mode, "null-bytes") == 0)
        view.bytes = NULL;
    if (strcmp(mode, "caret-over") == 0)
        caret = 3U;
    if (strcmp(mode, "syntax-low") == 0)
        syntax = (UmiEditorDelimiterSyntax)-1;
    if (strcmp(mode, "syntax-high") == 0)
        syntax = (UmiEditorDelimiterSyntax)9;
    if (strcmp(mode, "embedded-nul") == 0)
        view = View("(\0)", 3U);
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        view = View("\xc0\xaf()", 4U);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "depth-limit") == 0 || strcmp(mode, "depth-over") == 0)
    {
        size_t depth = strcmp(mode, "depth-limit") == 0 ? 512U : 513U;
        allocated = malloc(depth * 2U);
        CHECK(allocated != NULL);
        memset(allocated, '(', depth);
        memset(allocated + depth, ')', depth);
        view = View(allocated, depth * 2U);
        expected = depth == 512U ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "source-limit") == 0 || strcmp(mode, "source-over") == 0)
    {
        size_t size = 8U * 1024U * 1024U + (strcmp(mode, "source-over") == 0 ? 1U : 0U);
        allocated = malloc(size);
        CHECK(allocated != NULL);
        memset(allocated, ' ', size);
        allocated[0] = '(';
        allocated[size - 1U] = ')';
        view = View(allocated, size);
        expected = size == 8U * 1024U * 1024U ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiStatus status = UmiEditorDelimiterMatch(input, caret, syntax, result);
    free(allocated);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK)
        CHECK(memcmp(&output, &unchanged, sizeof(output)) == 0);
    else
        CHECK(output.opening_offset == 0U && output.closing_offset == view.byte_count - 1U);
    return 0;
}
