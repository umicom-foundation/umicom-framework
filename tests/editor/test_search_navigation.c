/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_search_navigation.c
 * PURPOSE: Check literal navigation and replacement without a GUI or result cap.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/search_engine.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/* Outputs use sentinel values to detect partial writes after a rejected call. */
int main(int argc, char **argv)
{
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 0, 1U};
    UmiEditorSearchMatch match = {99U, 99U};
    int wrapped = -1;
    char output[64] = "unchanged";
    size_t count = 99U;
    if (argc != 2) return 2;
    const char *name = argv[1];
    if (strcmp(name, "directions") == 0) {
        CHECK(UmiEditorSearchNavigate("one two one", 11U, "one", 3U, &options, 1U, 0, 0, &match, &wrapped) == UMI_STATUS_OK);
        CHECK(match.offset == 8U && !wrapped);
        CHECK(UmiEditorSearchNavigate("one two one", 11U, "one", 3U, &options, 8U, 1, 0, &match, &wrapped) == UMI_STATUS_OK);
        CHECK(match.offset == 0U && !wrapped);
    } else if (strcmp(name, "wrap") == 0) {
        CHECK(UmiEditorSearchNavigate("one two one", 11U, "one", 3U, &options, 0U, 1, 1, &match, &wrapped) == UMI_STATUS_OK);
        CHECK(match.offset == 8U && wrapped);
        CHECK(UmiEditorSearchNavigate("one two one", 11U, "one", 3U, &options, 11U, 0, 1, &match, &wrapped) == UMI_STATUS_OK);
        CHECK(match.offset == 0U && wrapped);
    } else if (strcmp(name, "failure-preserves-output") == 0) {
        CHECK(UmiEditorSearchNavigate("note", 4U, "memo", 4U, NULL, 0U, 0, 1, &match, &wrapped) == UMI_STATUS_NOT_FOUND);
        CHECK(match.offset == 99U && match.byte_count == 99U && wrapped == -1);
        CHECK(UmiEditorSearchNavigate(NULL, 0U, "memo", 4U, NULL, 0U, 0, 1, &match, NULL) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiEditorSearchNavigate("note", 4U, "note", 4U, NULL, 5U, 0, 1, &match, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        options.case_mode = (UmiEditorSearchCaseMode)17;
        CHECK(UmiEditorSearchNavigate("note", 4U, "note", 4U, &options, 0U, 0, 1, &match, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "word-boundaries") == 0) {
        options.whole_word = 1;
        const char *text = "notebook note note_1";
        CHECK(UmiEditorSearchNavigate(text, strlen(text), "note", 4U, &options, 0U, 0, 0, &match, NULL) == UMI_STATUS_OK && match.offset == 9U);
        CHECK(UmiEditorSearchReplaceAll(text, strlen(text), "note", 4U, "memo", 4U, &options, output, sizeof output, &count) == UMI_STATUS_OK);
        CHECK(count == 1U && strcmp(output, "notebook memo note_1") == 0);
    } else if (strcmp(name, "many-matches") == 0) {
        char text[6001], result[6001]; memset(text, 'a', sizeof text - 1U); text[6000] = '\0';
        CHECK(UmiEditorSearchNavigate(text, 6000U, "a", 1U, &options, 5999U, 0, 0, &match, NULL) == UMI_STATUS_OK && match.offset == 5999U);
        CHECK(UmiEditorSearchReplaceAll(text, 6000U, "a", 1U, "b", 1U, &options, result, sizeof result, &count) == UMI_STATUS_OK);
        CHECK(count == 6000U && result[0] == 'b' && result[5999] == 'b' && result[6000] == '\0');
    } else if (strcmp(name, "replace-capacity") == 0) {
        CHECK(UmiEditorSearchReplaceAll("aa", 2U, "a", 1U, "long", 4U, NULL, output, 8U, &count) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0 && count == 99U);
        CHECK(UmiEditorSearchReplaceAll("aa", 2U, "a", 1U, "long", 4U, NULL, output, 9U, &count) == UMI_STATUS_OK);
        CHECK(strcmp(output, "longlong") == 0 && count == 2U);
    } else if (strcmp(name, "replace-empty") == 0) {
        CHECK(UmiEditorSearchReplaceAll("note note", 9U, "note", 4U, NULL, 0U, NULL, output, sizeof output, &count) == UMI_STATUS_OK);
        CHECK(strcmp(output, " ") == 0 && count == 2U);
        CHECK(UmiEditorSearchReplaceAll(NULL, 0U, "note", 4U, NULL, 0U, NULL, output, 1U, &count) == UMI_STATUS_OK && output[0] == '\0' && count == 0U);
    } else if (strcmp(name, "replace-overlap") == 0) {
        options.allow_overlapping = 1;
        CHECK(UmiEditorSearchReplaceAll("aaaaa", 5U, "aa", 2U, "b", 1U, &options, output, sizeof output, &count) == UMI_STATUS_OK);
        CHECK(strcmp(output, "bba") == 0 && count == 2U);
    } else if (strcmp(name, "replace-smart-case") == 0) {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_SMART;
        CHECK(UmiEditorSearchReplaceAll("Note note NOTE", 14U, "note", 4U, "memo", 4U, &options, output, sizeof output, &count) == UMI_STATUS_OK);
        CHECK(strcmp(output, "memo memo memo") == 0 && count == 3U);
        CHECK(UmiEditorSearchReplaceAll("Note note NOTE", 14U, "Note", 4U, "memo", 4U, &options, output, sizeof output, &count) == UMI_STATUS_OK);
        CHECK(strcmp(output, "memo note NOTE") == 0 && count == 1U);
    } else if (strcmp(name, "replace-no-match") == 0) {
        CHECK(UmiEditorSearchReplaceAll("notes", 5U, "missing", 7U, "memo", 4U, NULL, output, sizeof output, &count) == UMI_STATUS_OK);
        CHECK(strcmp(output, "notes") == 0 && count == 0U);
        CHECK(UmiEditorSearchReplaceAll("notes", 5U, "", 0U, "memo", 4U, NULL, output, sizeof output, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "notes") == 0 && count == 0U);
    } else return 2;
    return 0;
}
