/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_snippet_review.c
 * PURPOSE: Exercise complete snippet proposals, linked values, approval expiry and shared document history without filesystem writes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/snippet_review.h"

static UmiStatus Cursor(ReplacementFixture *fixture, size_t offset, size_t selection)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, fixture->viewId, &view);
    if (status == UMI_STATUS_OK)
    {
        view.cursor_offset = offset;
        view.selection_length = selection;
        status = umi_ui_document_view_model_upsert(views, &view);
    }
    return status;
}
static UmiStatus ReadOnly(ReplacementFixture *fixture)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, fixture->viewId, &view);
    if (status == UMI_STATUS_OK)
    {
        view.read_only = 1;
        status = umi_ui_document_view_model_upsert(views, &view);
    }
    return status;
}
static UmiEditorSnippetTemplate Template(const char *body)
{
    UmiEditorSnippetTemplate snippet = {0};
    snippet.struct_size = (uint32_t)sizeof(snippet);
    snippet.api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    (void)snprintf(snippet.id, sizeof(snippet.id), "review");
    (void)snprintf(snippet.language_id, sizeof(snippet.language_id), "c");
    (void)snprintf(snippet.name, sizeof(snippet.name), "Reviewed insertion");
    (void)snprintf(snippet.body, sizeof(snippet.body), "%s", body);
    return snippet;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const char *cases[] = {"capture",
                           "insert",
                           "selection",
                           "empty",
                           "linked",
                           "unicode",
                           "multiline",
                           "final-stop",
                           "no-final",
                           "first-final",
                           "literal",
                           "empty-value",
                           "choices",
                           "owned",
                           "template-change",
                           "value-change",
                           "prepare-again",
                           "invalid-value",
                           "invalid-template",
                           "stale",
                           "caret",
                           "selection-change",
                           "read-only",
                           "create-read-only",
                           "closed",
                           "other-owner",
                           "other-tab",
                           "approval",
                           "revision",
                           "no-template",
                           "unprepared",
                           "cancel-template",
                           "cancel-value",
                           "cancel-prepare",
                           "after-apply",
                           "undo",
                           "redo",
                           "pending-typing",
                           "arguments"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture fixture = {0};
    const char *original = strcmp(name, "empty") == 0 ? "" : "left RIGHT";
    CHECK(Start(&fixture, original) == 0);
    if (strcmp(name, "pending-typing") == 0)
    {
        original = "left pending";
        CHECK(Draft(&fixture, original) == UMI_STATUS_OK);
    }
    size_t offset = original[0] == '\0' ? 0U : 5U;
    size_t selected = strcmp(name, "selection") == 0 ? 5U : 0U;
    CHECK(Cursor(&fixture, offset, selected) == UMI_STATUS_OK);
    UmiDocumentSnippetReview *review = NULL;
    if (strcmp(name, "create-read-only") == 0)
    {
        CHECK(ReadOnly(&fixture) == UMI_STATUS_OK);
        CHECK(UmiDocumentSnippetReviewCreate(fixture.documents, fixture.id, &review) ==
                  UMI_STATUS_PERMISSION_DENIED &&
              review == NULL);
        Stop(&fixture);
        return 0;
    }
    CHECK(UmiDocumentSnippetReviewCreate(fixture.documents, fixture.id, &review) == UMI_STATUS_OK);
    UmiDocumentSnippetSummary summary;
    CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK && summary.revision == 1U &&
          !summary.has_template);
    const char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiDocumentSnippetReviewOriginal(review, &text, &bytes) == UMI_STATUS_OK &&
          bytes == strlen(original) && strcmp(text, original) == 0);
    if (strcmp(name, "capture") == 0)
    {
        CHECK(ExpectText(&fixture, original) == 0);
        goto cleanup;
    }
    if (strcmp(name, "no-template") == 0)
    {
        CHECK(UmiDocumentSnippetReviewPrepare(fixture.documents, review, 1U, NULL) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentSnippetReviewValue(review, 1U, 1U, "x", 1U, NULL) == UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    const char *body = strcmp(name, "literal") == 0       ? "literal"
                       : strcmp(name, "final-stop") == 0  ? "a$0b"
                       : strcmp(name, "first-final") == 0 ? "a$0b$0c"
                       : strcmp(name, "no-final") == 0    ? "${1:name}"
                       : strcmp(name, "choices") == 0     ? "${1|one,two|}-$1$0"
                                                          : "${1:name}-$1$0";
    UmiEditorSnippetTemplate snippet = Template(body);
    CHECK(UmiDocumentSnippetReviewTemplate(review, 1U, &snippet, NULL) == UMI_STATUS_OK);
    CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK && summary.revision == 2U &&
          summary.has_template);
    if (strcmp(name, "unprepared") == 0)
    {
        CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, summary.revision, 1) ==
              UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    if (strcmp(name, "owned") == 0)
        memset(&snippet, 0, sizeof(snippet));
    const char *expanded = strcmp(name, "literal") == 0       ? "literal"
                           : strcmp(name, "final-stop") == 0  ? "ab"
                           : strcmp(name, "first-final") == 0 ? "abc"
                           : strcmp(name, "no-final") == 0    ? "name"
                           : strcmp(name, "choices") == 0     ? "one-one"
                                                              : "name-name";
    if (strcmp(name, "linked") == 0 || strcmp(name, "unicode") == 0 || strcmp(name, "multiline") == 0 ||
        strcmp(name, "empty-value") == 0)
    {
        const char *value = strcmp(name, "unicode") == 0       ? "caf\xc3\xa9"
                            : strcmp(name, "multiline") == 0   ? "a\nb"
                            : strcmp(name, "empty-value") == 0 ? ""
                                                               : "value";
        CHECK(UmiDocumentSnippetReviewValue(review, summary.revision, 1U, value, strlen(value), NULL) ==
              UMI_STATUS_OK);
        expanded = strcmp(name, "unicode") == 0       ? "caf\xc3\xa9-caf\xc3\xa9"
                   : strcmp(name, "multiline") == 0   ? "a\nb-a\nb"
                   : strcmp(name, "empty-value") == 0 ? "-"
                                                      : "value-value";
        CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK);
    }
    CHECK(UmiDocumentSnippetReviewExpanded(review, &text, &bytes) == UMI_STATUS_OK &&
          bytes == strlen(expanded) && strcmp(text, expanded) == 0);
    CHECK(UmiDocumentSnippetReviewPrepare(fixture.documents, review, summary.revision, NULL) ==
          UMI_STATUS_OK);
    CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK && summary.prepared &&
          summary.source.has_proposal);
    uint64_t approved = summary.revision;
    char expected[128];
    (void)snprintf(expected, sizeof(expected), "%.*s%s%s", (int)offset, original, expanded,
                   original + offset + selected);
    CHECK(UmiDocumentSnippetReviewProposed(review, &text, &bytes) == UMI_STATUS_OK &&
          bytes == strlen(expected) && strcmp(text, expected) == 0);
    size_t final_cursor =
        offset +
        ((strcmp(name, "final-stop") == 0 || strcmp(name, "first-final") == 0) ? 1U : strlen(expanded));
    CHECK(summary.source.proposed_cursor == final_cursor);
    CHECK(ExpectText(&fixture, original) == 0);
    if (strcmp(name, "template-change") == 0 || strcmp(name, "value-change") == 0)
    {
        if (strcmp(name, "template-change") == 0)
        {
            snippet = Template("other");
            CHECK(UmiDocumentSnippetReviewTemplate(review, approved, &snippet, NULL) == UMI_STATUS_OK);
        }
        else
            CHECK(UmiDocumentSnippetReviewValue(review, approved, 1U, "other", 5U, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentSnippetReviewProposed(review, &text, &bytes) == UMI_STATUS_INVALID_STATE &&
              text == NULL && bytes == 0U);
        CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved, 1) ==
              UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK && !summary.prepared &&
              !summary.source.has_proposal);
        CHECK(UmiDocumentSnippetReviewPrepare(fixture.documents, review, summary.revision, NULL) ==
              UMI_STATUS_OK);
        CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved, 1) == UMI_STATUS_BUSY);
        goto cleanup;
    }
    if (strcmp(name, "prepare-again") == 0)
    {
        CHECK(UmiDocumentSnippetReviewPrepare(fixture.documents, review, approved, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved, 1) == UMI_STATUS_BUSY);
        goto cleanup;
    }
    if (strcmp(name, "invalid-value") == 0 || strcmp(name, "invalid-template") == 0)
    {
        if (strcmp(name, "invalid-value") == 0)
/* Malformed UTF-8 is a parse failure in the shared text scanner. Keep the earlier expectation for review; the following assertion uses that contract and still checks that the prepared draft is unchanged. */
#if 0
            CHECK(UmiDocumentSnippetReviewValue(review, approved, 1U, "\xff", 1U, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
#endif
            CHECK(UmiDocumentSnippetReviewValue(review, approved, 1U, "\xff", 1U, NULL) ==
                  UMI_STATUS_PARSE_ERROR);
        else
        {
            snippet = Template("\xff");
/* Malformed UTF-8 is a parse failure in the shared text scanner. Keep the earlier expectation for review; the following assertion uses that contract and still checks that the prepared draft is unchanged. */
#if 0
            CHECK(UmiDocumentSnippetReviewTemplate(review, approved, &snippet, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
#endif
            CHECK(UmiDocumentSnippetReviewTemplate(review, approved, &snippet, NULL) ==
                  UMI_STATUS_PARSE_ERROR);
        }
        CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK &&
              summary.revision == approved && summary.prepared);
        CHECK(UmiDocumentSnippetReviewProposed(review, &text, &bytes) == UMI_STATUS_OK &&
              strcmp(text, expected) == 0);
    }
    if (strncmp(name, "cancel-", 7U) == 0)
    {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        UmiStatus cancelled =
            strcmp(name, "cancel-template") == 0
                ? UmiDocumentSnippetReviewTemplate(review, approved, &snippet, cancel)
            : strcmp(name, "cancel-value") == 0
                ? UmiDocumentSnippetReviewValue(review, approved, 1U, "later", 5U, cancel)
                : UmiDocumentSnippetReviewPrepare(fixture.documents, review, approved, cancel);
        umi_cancellation_token_destroy(cancel);
        CHECK(cancelled == UMI_STATUS_CANCELLED);
        CHECK(UmiDocumentSnippetReviewInspect(review, &summary) == UMI_STATUS_OK &&
              summary.revision == approved && summary.prepared);
    }
    if (strcmp(name, "approval") == 0)
    {
        CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved, 0) ==
              UMI_STATUS_PERMISSION_DENIED);
        goto cleanup;
    }
    if (strcmp(name, "revision") == 0)
    {
        CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved - 1U, 1) == UMI_STATUS_BUSY);
        goto cleanup;
    }
    if (strcmp(name, "other-owner") == 0)
    {
        ReplacementFixture other = {0};
        CHECK(Start(&other, "different") == 0);
        CHECK(UmiDocumentSnippetReviewApply(other.documents, review, approved, 1) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(ExpectText(&other, "different") == 0);
        Stop(&other);
        goto cleanup;
    }
    if (strcmp(name, "arguments") == 0)
    {
        CHECK(UmiDocumentSnippetReviewOriginal(NULL, &text, &bytes) == UMI_STATUS_INVALID_ARGUMENT &&
              text == NULL && bytes == 0U);
        CHECK(UmiDocumentSnippetReviewInspect(review, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSnippetReviewApply(NULL, review, approved, 1) == UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    UmiStatus status = UMI_STATUS_OK;
    if (strcmp(name, "stale") == 0)
    {
        CHECK(Draft(&fixture, "changed") == UMI_STATUS_OK);
        status = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(name, "caret") == 0 || strcmp(name, "selection-change") == 0)
    {
        CHECK(Cursor(&fixture, strcmp(name, "caret") == 0 ? 6U : 5U,
                     strcmp(name, "selection-change") == 0 ? 1U : 0U) == UMI_STATUS_OK);
        status = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(name, "read-only") == 0)
    {
        CHECK(ReadOnly(&fixture) == UMI_STATUS_OK);
        status = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(name, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(fixture.documents, 1) == UMI_STATUS_OK);
        status = UMI_STATUS_NOT_FOUND;
    }
    char other_view[UMI_UI_ID_CAPACITY] = {0};
    if (strcmp(name, "other-tab") == 0)
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", other_view, sizeof(other_view)) ==
              UMI_STATUS_OK);
    CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved, 1) == status);
    if (status == UMI_STATUS_OK)
    {
        CHECK(ExpectText(&fixture, expected) == 0);
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture.workbench), fixture.viewId,
                                              &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == final_cursor && view.selection_length == 0U);
        if (strcmp(name, "other-tab") == 0)
        {
            UmiDocumentWorkingCopySnapshot active;
            CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &active) == UMI_STATUS_OK &&
                  strcmp(active.view_id, other_view) == 0);
        }
        if (strcmp(name, "undo") == 0 || strcmp(name, "redo") == 0 || strcmp(name, "pending-typing") == 0)
        {
            CHECK(UmiDocumentCoordinatorUndo(fixture.documents, fixture.id) == UMI_STATUS_OK);
            CHECK(ExpectText(&fixture, original) == 0);
            if (strcmp(name, "redo") == 0)
            {
                CHECK(UmiDocumentCoordinatorRedo(fixture.documents, fixture.id) == UMI_STATUS_OK);
                CHECK(ExpectText(&fixture, expected) == 0);
            }
            if (strcmp(name, "pending-typing") == 0)
            {
                CHECK(UmiDocumentCoordinatorUndo(fixture.documents, fixture.id) == UMI_STATUS_OK);
                CHECK(ExpectText(&fixture, "left RIGHT") == 0);
            }
        }
        if (strcmp(name, "after-apply") == 0)
        {
            CHECK(UmiDocumentSnippetReviewApply(fixture.documents, review, approved, 1) ==
                  UMI_STATUS_INVALID_STATE);
            CHECK(UmiDocumentSnippetReviewTemplate(review, approved, &snippet, NULL) ==
                  UMI_STATUS_INVALID_STATE);
            CHECK(UmiDocumentSnippetReviewOriginal(review, &text, &bytes) == UMI_STATUS_OK &&
                  strcmp(text, original) == 0);
        }
    }
cleanup:
    UmiDocumentSnippetReviewDestroy(review);
    Stop(&fixture);
    return 0;
}
