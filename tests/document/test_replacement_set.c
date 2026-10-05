/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_set.c
 * PURPOSE: Verify complete replacement-set review gates and all-or-nothing draft edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/replacement_set.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {
        "capture",      "apply",        "review-all",        "partial-review",   "repeated-review",
        "approval",     "revision",     "stale-first",       "stale-last",       "closed",
        "read-only",    "empty-needle", "empty-replacement", "same-replacement", "no-match",
        "smart-case",   "sensitive",    "owned-inputs",      "untitled",         "pinned",
        "new-document", "undo",         "pending-typing",    "invalid-source",   "invalid-input",
        "consumed",     "arguments"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    ReplacementFixture first = {0}, second = {0};
    CHECK(Start(&first, "note NOTE") == 0);
    second = first;
    CHECK(umi_document_coordinator_new(first.documents, "second.c", second.viewId, sizeof(second.viewId)) ==
          UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &active) == UMI_STATUS_OK);
    second.id = active.document_id;
    CHECK(Draft(&second, "note second") == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(first.documents) == UMI_STATUS_OK);
    if (strcmp(mode, "pending-typing") == 0)
        CHECK(Draft(&first, "note pending") == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(first.workbench);
    if (strcmp(mode, "read-only") == 0 || strcmp(mode, "pinned") == 0)
    {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(views, second.viewId, &view) == UMI_STATUS_OK);
        if (strcmp(mode, "read-only") == 0)
            view.read_only = 1;
        else
            view.pinned = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "invalid-source") == 0)
        CHECK(Draft(&second, "bad\xff") == UMI_STATUS_OK);
    char needle[32] = "note", replacement[32] = "saved";
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(mode, "read-only") == 0)
        wanted = UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(mode, "invalid-source") == 0)
        wanted = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "invalid-input") == 0)
    {
        strcpy(needle, "bad\xff");
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "empty-needle") == 0)
    {
        needle[0] = '\0';
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "empty-replacement") == 0)
        replacement[0] = '\0';
    if (strcmp(mode, "same-replacement") == 0)
    {
        strcpy(needle, "NOTE");
        strcpy(replacement, "NOTE");
    }
    if (strcmp(mode, "no-match") == 0)
        strcpy(needle, "missing");
    if (strcmp(mode, "sensitive") == 0)
        strcpy(needle, "NOTE");
    UmiDocumentReplacementSet *set = NULL;
    CHECK(UmiDocumentReplacementSetCreate(first.documents, needle, replacement, &set) == wanted);
    if (wanted != UMI_STATUS_OK)
    {
        CHECK(set == NULL && ExpectText(&first, "note NOTE") == 0);
        goto cleanup;
    }
    UmiDocumentReplacementSetSummary summary;
    CHECK(UmiDocumentReplacementSetInspect(set, &summary) == UMI_STATUS_OK && summary.document_count == 2U);
    size_t expected_matches = strcmp(mode, "no-match") == 0                                             ? 0U
                              : strcmp(mode, "same-replacement") == 0 || strcmp(mode, "sensitive") == 0 ? 1U
                              : strcmp(mode, "pending-typing") == 0                                     ? 2U
                                                                                                        : 3U;
    CHECK(summary.match_count == expected_matches);
    if (strcmp(mode, "untitled") == 0)
        CHECK(!active.has_path);
    size_t expected_changed = strcmp(mode, "same-replacement") == 0 || strcmp(mode, "no-match") == 0 ? 0U
                              : strcmp(mode, "sensitive") == 0                                       ? 1U
                                                                                                     : 2U;
    CHECK(summary.changed_count == expected_changed && summary.reviewed_count == 0U);
    const char *expected_first = strcmp(mode, "empty-replacement") == 0 ? " "
                                 : strcmp(mode, "same-replacement") == 0 || strcmp(mode, "no-match") == 0
                                     ? "note NOTE"
                                 : strcmp(mode, "sensitive") == 0      ? "note saved"
                                 : strcmp(mode, "pending-typing") == 0 ? "saved pending"
                                                                       : "saved saved";
    const char *expected_second = strcmp(mode, "empty-replacement") == 0                     ? " second"
                                  : expected_changed == 0U || strcmp(mode, "sensitive") == 0 ? "note second"
                                                                                             : "saved second";
    const char *before = NULL, *after = NULL;
    size_t before_bytes = 0U, after_bytes = 0U;
    CHECK(UmiDocumentReplacementSetTexts(set, 0U, &before, &before_bytes, &after, &after_bytes) ==
              UMI_STATUS_OK &&
          strcmp(after, expected_first) == 0);
    if (strcmp(mode, "owned-inputs") == 0)
    {
        strcpy(needle, "later");
        strcpy(replacement, "wrong");
        CHECK(strcmp(after, expected_first) == 0);
    }
    if (strcmp(mode, "capture") == 0)
    {
        CHECK(ExpectText(&first, "note NOTE") == 0 && ExpectText(&second, "note second") == 0);
        goto cleanup;
    }
    uint64_t revision = summary.revision;
    int approved = 1;
    if (strcmp(mode, "approval") == 0)
    {
        approved = 0;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "revision") == 0)
    {
        --revision;
        wanted = UMI_STATUS_BUSY;
    }
    if (strcmp(mode, "new-document") == 0)
        CHECK(umi_document_coordinator_new(first.documents, "later.c", NULL, 0U) == UMI_STATUS_OK);
    if (expected_changed != 0U)
        CHECK(UmiDocumentReplacementSetApply(set, summary.revision, 1) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiDocumentReplacementSetReview(set, 0U, summary.revision) == UMI_STATUS_OK);
    if (strcmp(mode, "repeated-review") == 0)
        CHECK(UmiDocumentReplacementSetReview(set, 0U, summary.revision) == UMI_STATUS_OK);
    if (strcmp(mode, "partial-review") != 0)
        CHECK(UmiDocumentReplacementSetReview(set, 1U, summary.revision) == UMI_STATUS_OK);
    else
        wanted = UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(mode, "stale-first") == 0)
    {
        CHECK(Draft(&first, "later") == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "stale-last") == 0)
    {
        CHECK(Draft(&second, "later") == UMI_STATUS_OK);
        wanted = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(first.documents, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    CHECK(UmiDocumentReplacementSetApply(set, revision, approved) == wanted);
    if (wanted == UMI_STATUS_OK)
    {
        CHECK(ExpectText(&first, expected_first) == 0 && ExpectText(&second, expected_second) == 0);
        CHECK(UmiDocumentReplacementSetInspect(set, &summary) == UMI_STATUS_OK && summary.applied &&
              summary.reviewed_count == summary.changed_count);
        if (strcmp(mode, "undo") == 0 || strcmp(mode, "pending-typing") == 0)
        {
            CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK);
            CHECK(ExpectText(&first, strcmp(mode, "pending-typing") == 0 ? "note pending" : "note NOTE") ==
                      0 &&
                  ExpectText(&second, expected_second) == 0);
        }
        if (strcmp(mode, "consumed") == 0)
        {
            CHECK(UmiDocumentReplacementSetApply(set, revision, 1) == UMI_STATUS_INVALID_STATE);
            CHECK(UmiDocumentReplacementSetReview(set, 0U, revision) == UMI_STATUS_INVALID_STATE);
            CHECK(UmiDocumentReplacementSetTexts(set, 0U, &before, &before_bytes, &after, &after_bytes) ==
                      UMI_STATUS_INVALID_STATE &&
                  before == NULL && after == NULL);
        }
    }
    else
    {
        CHECK(ExpectText(&first, strcmp(mode, "stale-first") == 0 ? "later" : "note NOTE") == 0);
        if (strcmp(mode, "closed") != 0)
            CHECK(ExpectText(&second, strcmp(mode, "stale-last") == 0 ? "later" : "note second") == 0);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        UmiDocumentReplacementSet *other = (UmiDocumentReplacementSet *)1;
        CHECK(UmiDocumentReplacementSetCreate(NULL, "a", "b", &other) == UMI_STATUS_INVALID_ARGUMENT &&
              other == NULL);
        CHECK(UmiDocumentReplacementSetCreate(first.documents, NULL, "b", &other) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentReplacementSetCreate(first.documents, "a", NULL, &other) ==
              UMI_STATUS_INVALID_ARGUMENT);
        UmiDocumentSourceRequestSummary document;
        size_t matches = 99U;
        int reviewed = 0;
        CHECK(UmiDocumentReplacementSetAt(set, 2U, &document, &matches, &reviewed) == UMI_STATUS_NOT_FOUND &&
              matches == 99U);
        CHECK(UmiDocumentReplacementSetTexts(set, 2U, &before, &before_bytes, &after, &after_bytes) ==
                  UMI_STATUS_NOT_FOUND &&
              before == NULL && after == NULL);
        UmiDocumentReplacementSetDestroy(NULL);
    }
cleanup:
    UmiDocumentReplacementSetDestroy(set);
    Stop(&first);
    return 0;
}
