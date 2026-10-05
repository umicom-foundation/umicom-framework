/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_search_options.c
 * PURPOSE: Check explicit literal navigation and replacement against complete document word boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/search.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1],
               *cases[] = {"sensitive",           "insensitive",     "smart-upper",       "smart-lower",
                           "substring",           "whole-word",      "next-selection",    "backwards",
                           "wrap-forward",        "wrap-backward",   "read-only",         "invalid-case",
                           "invalid-word",        "default-options", "replace-selection", "replace-boundary",
                           "replace-ignore-case", "replace-empty",   "replace-read-only", "replace-missing",
                           "replace-default"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    const char *source = "foobar foo FOO foo_ foo2 foo";
    CHECK(Start(&f, source) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 1, 0, 0U};
    const UmiEditorSearchOptions *input = &options;
    const char *needle = "foo", *replacement = "bar";
    size_t expected = 7U, offset = 99U;
    int backwards = 0, wrapped = 91, expectedWrap = 0;
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(name, "sensitive") == 0)
    {
        view.cursor_offset = 11U;
        expected = 25U;
    }
    if (strcmp(name, "insensitive") == 0)
    {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
        needle = "FOO";
    }
    if (strcmp(name, "smart-upper") == 0)
    {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_SMART;
        needle = "FOO";
        expected = 11U;
    }
    if (strcmp(name, "smart-lower") == 0)
    {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_SMART;
        view.cursor_offset = 11U;
        expected = 11U;
    }
    if (strcmp(name, "substring") == 0)
    {
        options.whole_word = 0;
        expected = 0U;
    }
    if (strcmp(name, "next-selection") == 0)
    {
        view.cursor_offset = 7U;
        view.selection_length = 3U;
        expected = 25U;
    }
    if (strcmp(name, "backwards") == 0)
    {
        view.cursor_offset = 25U;
        backwards = 1;
    }
    if (strcmp(name, "wrap-forward") == 0)
    {
        view.cursor_offset = strlen(source);
        expectedWrap = 1;
    }
    if (strcmp(name, "wrap-backward") == 0)
    {
        backwards = 1;
        expected = 25U;
        expectedWrap = 1;
    }
    if (strcmp(name, "read-only") == 0 || strcmp(name, "replace-read-only") == 0)
        view.read_only = 1;
    if (strcmp(name, "invalid-case") == 0)
    {
        options.case_mode = (UmiEditorSearchCaseMode)19;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "invalid-word") == 0)
    {
        options.whole_word = 2;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "default-options") == 0)
    {
        input = NULL;
        expected = 0U;
    }
    int editing = strncmp(name, "replace-", 8U) == 0;
    const char *changed = "foobar bar FOO foo_ foo2 foo";
    if (editing)
    {
        view.cursor_offset = 7U;
        view.selection_length = 3U;
        if (strcmp(name, "replace-boundary") == 0)
            view.cursor_offset = 0U;
        if (strcmp(name, "replace-ignore-case") == 0)
        {
            view.cursor_offset = 11U;
            options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
            changed = "foobar foo bar foo_ foo2 foo";
            expected = 11U;
        }
        if (strcmp(name, "replace-empty") == 0)
        {
            replacement = "";
            changed = "foobar  FOO foo_ foo2 foo";
        }
        if (strcmp(name, "replace-read-only") == 0)
            wanted = UMI_STATUS_PERMISSION_DENIED;
        if (strcmp(name, "replace-missing") == 0)
        {
            needle = "absent";
            wanted = UMI_STATUS_NOT_FOUND;
        }
        if (strcmp(name, "replace-default") == 0)
        {
            input = NULL;
            view.cursor_offset = 0U;
            expected = 0U;
            changed = "barbar foo FOO foo_ foo2 foo";
        }
    }
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
    UmiStatus status =
        editing
            ? UmiDocumentCoordinatorReplaceNextWithOptions(f.documents, needle, replacement, input, &offset)
            : UmiDocumentCoordinatorFindWithOptions(f.documents, needle, input, backwards, &offset, &wrapped);
    CHECK(status == wanted);
    if (status == UMI_STATUS_OK)
    {
        CHECK(offset == expected);
        if (editing)
        {
            CHECK(ExpectText(&f, changed) == 0);
            CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, source) == 0);
        }
        else
        {
            CHECK(wrapped == expectedWrap);
            CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
            CHECK(view.cursor_offset == expected && view.selection_length == 3U);
        }
    }
    else
        CHECK(offset == 99U && wrapped == 91);
    if (!editing || status != UMI_STATUS_OK)
    {
        CHECK(ExpectText(&f, source) == 0);
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
        CHECK(before.undo_count == after.undo_count && before.dirty == after.dirty);
    }
    Stop(&f);
    return 0;
}
