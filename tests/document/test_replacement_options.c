/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_options.c
 * PURPOSE: Verify all replacement-review owners capture the same explicit case and word policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/replacement_session.h"
#include "umicom/document/replacement_set.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1],
               *cases[] = {"plan-sensitive",  "plan-insensitive", "plan-smart",     "plan-owned",
                           "plan-complete",   "plan-invalid",     "session-policy", "session-owned",
                           "session-invalid", "set-policy",       "set-owned",      "set-complete",
                           "set-invalid"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    const char *source = "foobar foo FOO foo_ foo2 foo";
    CHECK(Start(&f, source) == 0);
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 1, 0, 0U};
    const char *needle = "foo", *expectedText = "foobar bar FOO foo_ foo2 bar";
    size_t matches = 2U;
    UmiStatus wanted = UMI_STATUS_OK;
    if (strstr(name, "invalid") != NULL)
    {
        options.whole_word = -1;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "plan-insensitive") == 0)
    {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
        matches = 3U;
        expectedText = "foobar bar bar foo_ foo2 bar";
    }
    if (strcmp(name, "plan-smart") == 0)
    {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_SMART;
        needle = "FOO";
        matches = 1U;
        expectedText = "foobar foo bar foo_ foo2 foo";
    }
    if (strstr(name, "complete") != NULL)
    {
        options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
        options.whole_word = 0;
        options.allow_overlapping = 1;
        options.maximum_matches = 1U;
        matches = 6U;
        expectedText = "barbar bar bar bar_ bar2 bar";
    }
    if (strncmp(name, "plan-", 5U) == 0)
    {
        CHECK(UmiDocumentCoordinatorPrepareReplacementWithOptions(f.documents, f.id, needle, "bar", &options,
                                                                  &f.plan) == wanted);
        if (wanted != UMI_STATUS_OK)
            CHECK(f.plan == NULL);
        else
        {
            if (strstr(name, "owned") != NULL)
            {
                options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
                options.whole_word = 0;
            }
            CHECK(ExpectPlan(&f, source, expectedText, matches) == 0);
            CHECK(ExpectText(&f, source) == 0);
            CHECK(UmiDocumentCoordinatorApplyReplacement(f.documents, f.plan, NULL) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, expectedText) == 0);
            CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, source) == 0);
        }
    }
    else if (strncmp(name, "session-", 8U) == 0)
    {
        UmiDocumentReplacementSession *session = NULL;
        CHECK(UmiDocumentReplacementSessionCreateWithOptions(f.documents, needle, "bar", &options,
                                                             &session) == wanted);
        if (wanted != UMI_STATUS_OK)
            CHECK(session == NULL);
        else
        {
            if (strstr(name, "owned") != NULL)
            {
                options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
                options.whole_word = 0;
            }
            CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
            UmiDocumentReplacementProgress progress;
            CHECK(UmiDocumentReplacementSessionProgress(session, &progress) == UMI_STATUS_OK &&
                  progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW &&
                  progress.current.match_count == matches);
            CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_APPLY) ==
                  UMI_STATUS_OK);
            CHECK(ExpectText(&f, expectedText) == 0);
        }
        UmiDocumentReplacementSessionDestroy(session);
    }
    else
    {
        UmiDocumentReplacementSet *set = NULL;
        CHECK(UmiDocumentReplacementSetCreateWithOptions(f.documents, needle, "bar", &options, &set) ==
              wanted);
        if (wanted != UMI_STATUS_OK)
            CHECK(set == NULL);
        else
        {
            if (strstr(name, "owned") != NULL)
            {
                options.case_mode = UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE;
                options.whole_word = 0;
            }
            UmiDocumentReplacementSetSummary summary;
            CHECK(UmiDocumentReplacementSetInspect(set, &summary) == UMI_STATUS_OK &&
                  summary.match_count == matches);
            CHECK(UmiDocumentReplacementSetReview(set, 0U, summary.revision) == UMI_STATUS_OK);
            CHECK(UmiDocumentReplacementSetApply(set, summary.revision, 1) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, expectedText) == 0);
        }
        UmiDocumentReplacementSetDestroy(set);
    }
    Stop(&f);
    return 0;
}
