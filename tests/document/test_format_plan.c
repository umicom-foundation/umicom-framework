/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_format_plan.c
 * PURPOSE: Check exact source conversion, Unicode selection mapping and pre-publication refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"lf",
                                            "crlf",
                                            "cr",
                                            "mixed",
                                            "unicode",
                                            "selection",
                                            "eof",
                                            "final",
                                            "empty",
                                            "empty-final",
                                            "final-present",
                                            "preserve",
                                            "owned",
                                            "no-change",
                                            "invalid-utf8",
                                            "embedded-null",
                                            "split-scalar",
                                            "split-crlf",
                                            "invalid-cursor",
                                            "invalid-selection",
                                            "invalid-target",
                                            "invalid-final",
                                            "preserve-final",
                                            "input-limit",
                                            "growth-limit",
                                            "cancelled",
                                            "null-output"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    char source[64] = "a\r\nb\rc\nd", *large = NULL;
    const char *input = source, *expected = "a\nb\nc\nd";
    size_t bytes = strlen(source), cursor = 3U, selected = 3U, wanted_cursor = 2U, wanted_selection = 3U;
    UmiDocumentLineEnding target = UMI_DOCUMENT_LINE_ENDING_LF;
    int final = 0;
    UmiStatus wanted = UMI_STATUS_OK;
    UmiCancellationToken *cancel = NULL;
    if (strcmp(mode, "crlf") == 0)
    {
        target = UMI_DOCUMENT_LINE_ENDING_CRLF;
        expected = "a\r\nb\r\nc\r\nd";
        wanted_cursor = 3U;
        wanted_selection = 4U;
    }
    if (strcmp(mode, "cr") == 0)
    {
        target = UMI_DOCUMENT_LINE_ENDING_CR;
        expected = "a\rb\rc\rd";
    }
    if (strcmp(mode, "unicode") == 0)
    {
        strcpy(source, "\xe9\x9b\xaa\r\n\xf0\x9f\x98\x80\r\nx");
        bytes = strlen(source);
        cursor = 5U;
        selected = 6U;
        expected = "\xe9\x9b\xaa\n\xf0\x9f\x98\x80\nx";
        wanted_cursor = 4U;
        wanted_selection = 5U;
    }
    if (strcmp(mode, "selection") == 0)
    {
        cursor = 0U;
        selected = bytes;
        wanted_cursor = 0U;
        wanted_selection = strlen(expected);
    }
    if (strcmp(mode, "eof") == 0)
    {
        cursor = bytes;
        selected = 0U;
        wanted_cursor = strlen(expected);
        wanted_selection = 0U;
    }
    if (strcmp(mode, "final") == 0)
    {
        final = 1;
        cursor = bytes;
        selected = 0U;
        expected = "a\nb\nc\nd\n";
        wanted_cursor = 7U;
        wanted_selection = 0U;
    }
    if (strcmp(mode, "empty") == 0 || strcmp(mode, "empty-final") == 0)
    {
        source[0] = '\0';
        bytes = cursor = selected = wanted_cursor = wanted_selection = 0U;
        final = strcmp(mode, "empty-final") == 0;
        expected = final ? "\n" : "";
    }
    if (strcmp(mode, "no-change") == 0 || strcmp(mode, "final-present") == 0)
    {
        strcpy(source, "a\nb\n");
        bytes = strlen(source);
        cursor = 2U;
        selected = 1U;
        wanted_cursor = 2U;
        wanted_selection = 1U;
        expected = source;
        final = strcmp(mode, "final-present") == 0;
    }
    if (strcmp(mode, "preserve") == 0)
    {
        target = UMI_DOCUMENT_LINE_ENDING_NONE;
        expected = source;
        wanted_cursor = cursor;
        wanted_selection = selected;
    }
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        source[4] = (char)0xff;
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "embedded-null") == 0)
    {
        source[4] = '\0';
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "split-scalar") == 0)
    {
        strcpy(source, "\xe9\x9b\xaa");
        bytes = 3U;
        cursor = 1U;
        selected = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "split-crlf") == 0)
    {
        cursor = 2U;
        selected = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-cursor") == 0)
    {
        cursor = bytes + 1U;
        selected = 0U;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-selection") == 0)
    {
        selected = SIZE_MAX;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-target") == 0)
    {
        target = UMI_DOCUMENT_LINE_ENDING_MIXED;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-final") == 0)
    {
        final = 2;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "preserve-final") == 0)
    {
        target = UMI_DOCUMENT_LINE_ENDING_NONE;
        final = 1;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "input-limit") == 0)
    {
        bytes = UMI_DOCUMENT_FORMAT_TEXT_LIMIT + 1U;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "growth-limit") == 0)
    {
        bytes = UMI_DOCUMENT_FORMAT_TEXT_LIMIT;
        large = malloc(bytes);
        CHECK(large);
        memset(large, '\n', bytes);
        input = large;
        cursor = selected = 0U;
        target = UMI_DOCUMENT_LINE_ENDING_CRLF;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "cancelled") == 0)
    {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "null-output") == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentFormatPlan *plan = NULL;
    UmiStatus status = UmiDocumentFormatPlanCreate(input, bytes, cursor, selected, target, final, cancel,
                                                   strcmp(mode, "null-output") == 0 ? NULL : &plan);
    CHECK(status == wanted);
    if (status == UMI_STATUS_OK)
    {
        UmiDocumentFormatSummary summary;
        const char *text = NULL;
        size_t length = 0U;
        CHECK(UmiDocumentFormatPlanInspect(plan, &summary) == UMI_STATUS_OK);
        if (strcmp(mode, "owned") == 0)
            memset(source, 'x', bytes);
        CHECK(UmiDocumentFormatPlanRead(plan, &text, &length) == UMI_STATUS_OK &&
              length == strlen(expected) && memcmp(text, expected, length) == 0);
        CHECK(summary.cursor_offset == wanted_cursor && summary.selection_bytes == wanted_selection &&
              summary.proposed_bytes == length);
        if (strcmp(mode, "no-change") == 0 || strcmp(mode, "final-present") == 0 ||
            strcmp(mode, "preserve") == 0)
            CHECK(!summary.text_changes);
        if (strcmp(mode, "final") == 0 || strcmp(mode, "empty-final") == 0)
            CHECK(summary.added_final_newline);
        if (strcmp(mode, "mixed") == 0)
            CHECK(summary.replaced_endings == 2U);
    }
    else
        CHECK(plan == NULL);
    UmiDocumentFormatPlanDestroy(plan);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
