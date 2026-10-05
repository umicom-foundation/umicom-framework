/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/source_review/test_workspace_edit.c
 * PURPOSE: Exercise complete edit approval with untouched dependencies, annotations and real document Undo.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../document/source_workspace_fixture.h"
#include "umicom/source_review/workspace_edit.h"
#include <stdint.h>
/* The fixture uses coordinator-generated untitled URIs, which contain no JSON
 * quote or escape characters. Keep provider text literals independent of the
 * production serializer so the review exercises real catalogue decoding. */
static int Proposal(const char *mode, const char *first_uri, const char *second_uri,
                    UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    char json[8192], first[3000], second[3000];
    const char *after =
        strncmp(mode, "annotation-", 11U) == 0 || strcmp(mode, "annotation") == 0 ? "changed" : "changed";
    if (strcmp(mode, "unchanged") == 0 || strcmp(mode, "annotation-noop") == 0)
        after = "source";
    if (strcmp(mode, "empty-replacement") == 0)
        after = "";
    if (strcmp(mode, "unicode") == 0)
        after = "caf\xc3\xa9";
    int annotations = strncmp(mode, "annotation", 10U) == 0;
    int version = strcmp(mode, "version-stale") == 0 ? 2 : 1;
    unsigned end = strcmp(mode, "invalid-range") == 0 ? 99U : 6U;
    const char *marker = annotations ? ",\"annotationId\":\"reason\"" : "";
    int length = snprintf(
        first, sizeof(first),
        "{\"textDocument\":{\"uri\":\"%s\",\"version\":%d},\"edits\":[{\"range\":{\"start\":{\"line\":0,"
        "\"character\":0},\"end\":{\"line\":0,\"character\":%u}},\"newText\":\"%s\"%s}%s]}",
        first_uri, version, end, after, marker,
        strcmp(mode, "overlap") == 0 ? ",{\"range\":{\"start\":{\"line\":0,\"character\":1},\"end\":{"
                                       "\"line\":0,\"character\":2}},\"newText\":\"x\"}"
                                     : "");
    CHECK(length >= 0 && (size_t)length < sizeof(first));
    const char *second_target = strcmp(mode, "unknown-target") == 0 ? "file:///not-open.c" : second_uri;
    length =
        snprintf(second, sizeof(second),
                 "{\"textDocument\":{\"uri\":\"%s\",\"version\":1},\"edits\":[{\"range\":{\"start\":{"
                 "\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":6}},\"newText\":\"%s\"%s}]}",
                 second_target, after, strcmp(mode, "annotation-shared") == 0 ? marker : "");
    CHECK(length >= 0 && (size_t)length < sizeof(second));
    const char *annotation_json = "";
    if (annotations)
        annotation_json =
            strcmp(mode, "annotation-optional") == 0
                ? ",\"changeAnnotations\":{\"reason\":{\"label\":\"Review name\",\"description\":\"A "
                  "complete explanation\",\"needsConfirmation\":false}}"
                : ",\"changeAnnotations\":{\"reason\":{\"label\":\"Review name\",\"description\":\"A "
                  "complete explanation\",\"needsConfirmation\":true},\"unused\":{\"label\":\"Unused "
                  "explanation\",\"needsConfirmation\":true}}";
    if (strcmp(mode, "reverse-order") == 0)
        length =
            snprintf(json, sizeof(json), "{\"documentChanges\":[%s,%s]%s}", second, first, annotation_json);
    else if (strcmp(mode, "one-target") == 0)
        length = snprintf(json, sizeof(json), "{\"documentChanges\":[%s]}", first);
    else
        length =
            snprintf(json, sizeof(json), "{\"documentChanges\":[%s,%s]%s}", first, second, annotation_json);
    CHECK(length >= 0 && (size_t)length < sizeof(json));
    if (strcmp(mode, "empty") == 0)
        strcpy(json, "{}");
    CHECK(UmiLanguageWorkspaceEditCatalogueCreate(json, strlen(json), NULL, out_catalogue) == UMI_STATUS_OK);
    memset(json, 'x', sizeof(json));
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"two-targets",
                           "one-target",
                           "reverse-order",
                           "capture",
                           "ownership",
                           "unknown-target",
                           "empty",
                           "version",
                           "version-unknown",
                           "version-stale",
                           "invalid-range",
                           "overlap",
                           "read-only-target",
                           "read-only-dependency",
                           "stale-before",
                           "stale-target",
                           "stale-dependency",
                           "dependency-round-trip",
                           "dependency-caret",
                           "dependency-permission",
                           "dependency-saved",
                           "dependency-closed",
                           "review-none",
                           "review-partial",
                           "review-repeat",
                           "approval",
                           "revision",
                           "annotation",
                           "annotation-missing",
                           "annotation-revoke",
                           "annotation-repeat",
                           "annotation-unused",
                           "annotation-optional",
                           "annotation-noop",
                           "annotation-shared",
                           "unicode",
                           "empty-replacement",
                           "unchanged",
                           "pending-typing",
                           "undo",
                           "redo",
                           "active-tab",
                           "new-document",
                           "consumed",
                           "arguments",
                           "cancelled"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    ReplacementFixture first = {0}, second = {0}, dependency = {0};
    CHECK(Start(&first, "source one") == 0 &&
          SourceFixtureAdd(&first, &second, "second.c", "source two") == 0 &&
          SourceFixtureAdd(&first, &dependency, "dependency.h", "dependency") == 0);
    const char *before_first = "source one", *before_second = "source two";
    if (strcmp(mode, "pending-typing") == 0)
    {
        before_first = "source draft one";
        before_second = "source draft two";
        CHECK(Draft(&first, before_first) == UMI_STATUS_OK && Draft(&second, before_second) == UMI_STATUS_OK);
    }
    UmiUiDocumentViewSnapshot first_view, second_view, dependency_view;
    CHECK(SourceFixtureView(&first, &first_view) == 0 && SourceFixtureView(&second, &second_view) == 0 &&
          SourceFixtureView(&dependency, &dependency_view) == 0);
    if (strcmp(mode, "read-only-target") == 0)
    {
        second_view.read_only = 1;
        CHECK(SourceFixturePut(&second, &second_view) == 0);
    }
    if (strcmp(mode, "read-only-dependency") == 0)
    {
        dependency_view.read_only = 1;
        CHECK(SourceFixturePut(&dependency, &dependency_view) == 0);
    }
    UmiDocumentId ids[3] = {first.id, second.id, dependency.id};
    UmiDocumentSourceWorkspace *sources = NULL;
    CHECK(UmiDocumentSourceWorkspaceCreate(first.documents, ids, 3U, &sources) == UMI_STATUS_OK);
    UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    CHECK(Proposal(mode, first_view.uri, second_view.uri, &catalogue) == 0);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    int32_t version = 1;
    const int32_t *known_version = &version;
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "version-unknown") == 0)
    {
        known_version = NULL;
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "version-stale") == 0 || strcmp(mode, "overlap") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(mode, "invalid-range") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "read-only-target") == 0)
        expected = UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(mode, "unknown-target") == 0 || strcmp(mode, "empty") == 0)
        expected = UMI_STATUS_NOT_FOUND;
    if (strcmp(mode, "stale-before") == 0)
    {
        CHECK(Draft(&dependency, "changed dependency") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    UmiDocumentSourceWorkspace *original_sources = sources;
    UmiLanguageWorkspaceEditCatalogue *original_catalogue = catalogue;
    UmiSourceWorkspaceEditReview *review = (UmiSourceWorkspaceEditReview *)1;
    CHECK(UmiSourceWorkspaceEditReviewCreate(first.documents, &sources, &catalogue, known_version, cancel,
                                             &review) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(review == NULL && sources == original_sources && catalogue == original_catalogue);
        CHECK(ExpectText(&first, before_first) == 0 && ExpectText(&second, before_second) == 0);
        goto cleanup;
    }
    CHECK(sources == NULL && catalogue == NULL);
    UmiSourceWorkspaceEditReviewSummary summary;
    CHECK(UmiSourceWorkspaceEditReviewInspect(review, &summary) == UMI_STATUS_OK &&
          summary.dependency_count == 3U);
    size_t documents = strcmp(mode, "one-target") == 0 ? 1U : 2U;
    CHECK(summary.document_count == documents && summary.reviewed_count == 0U && !summary.applied);
    /* The mutable input pointer has transferred to the review. Inspect through
     * the const catalogue interface below; retain the initial attempt for review. */
#if 0
    CHECK(UmiSourceWorkspaceEditReviewCatalogue(review,&original_catalogue)==UMI_STATUS_OK);
#endif
    /* Resolve the review's borrowed catalogue through a const owner below; the
     * caller no longer destroys it after the successful ownership transfer. */
    const UmiLanguageWorkspaceEditCatalogue *borrowed = NULL;
    CHECK(UmiSourceWorkspaceEditReviewCatalogue(review, &borrowed) == UMI_STATUS_OK && borrowed != NULL);
    for (size_t i = 0U; i < documents; ++i)
    {
        UmiDocumentSourceRequestSummary document;
        int reviewed = 1;
        CHECK(UmiSourceWorkspaceEditReviewAt(review, i, &document, &reviewed) == UMI_STATUS_OK && !reviewed);
        size_t first_index = strcmp(mode, "reverse-order") == 0 ? 1U : 0U;
        CHECK(document.document_id == (i == first_index ? first.id : second.id));
        const char *source = NULL, *proposed = NULL;
        size_t source_bytes = 0U, proposed_bytes = 0U;
        CHECK(UmiSourceWorkspaceEditReviewTexts(review, i, &source, &source_bytes, &proposed,
                                                &proposed_bytes) == UMI_STATUS_OK);
        CHECK(source_bytes == strlen(i == first_index ? before_first : before_second));
        CHECK(proposed_bytes == strlen(proposed) && source != proposed);
        if (strcmp(mode, "review-none") == 0 || (strcmp(mode, "review-partial") == 0 && i == documents - 1U))
            continue;
        CHECK(UmiSourceWorkspaceEditReviewAcceptDocument(review, i, summary.revision) == UMI_STATUS_OK);
        if (strcmp(mode, "review-repeat") == 0)
            CHECK(UmiSourceWorkspaceEditReviewAcceptDocument(review, i, summary.revision) == UMI_STATUS_OK);
    }
    for (size_t i = 0U; i < UmiLanguageWorkspaceEditCatalogueAnnotationCount(borrowed); ++i)
    {
        int required = 0, confirmed = 1;
        UmiLanguageWorkspaceChangeAnnotation annotation;
        CHECK(UmiSourceWorkspaceEditReviewAnnotationState(review, i, &required, &confirmed) ==
                  UMI_STATUS_OK &&
              !confirmed);
        CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(borrowed, i, &annotation) == UMI_STATUS_OK);
        CHECK(required == (strcmp(annotation.id, "reason") == 0 && strcmp(mode, "annotation-optional") != 0));
        if (!required)
        {
            CHECK(UmiSourceWorkspaceEditReviewConfirmAnnotation(review, i, summary.revision, 1) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            continue;
        }
        CHECK(strcmp(annotation.label, "Review name") == 0 &&
              strcmp(annotation.description, "A complete explanation") == 0);
        if (strcmp(mode, "annotation-missing") == 0)
            continue;
        CHECK(UmiSourceWorkspaceEditReviewConfirmAnnotation(review, i, summary.revision, 1) == UMI_STATUS_OK);
        if (strcmp(mode, "annotation-repeat") == 0)
            CHECK(UmiSourceWorkspaceEditReviewConfirmAnnotation(review, i, summary.revision, 1) ==
                  UMI_STATUS_OK);
        if (strcmp(mode, "annotation-revoke") == 0)
            CHECK(UmiSourceWorkspaceEditReviewConfirmAnnotation(review, i, summary.revision, 0) ==
                  UMI_STATUS_OK);
    }
    uint64_t revision = summary.revision;
    int approved = 1;
    expected = UMI_STATUS_OK;
    if (strcmp(mode, "review-none") == 0 || strcmp(mode, "review-partial") == 0 ||
        strcmp(mode, "annotation-missing") == 0 || strcmp(mode, "annotation-revoke") == 0)
        expected = UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(mode, "approval") == 0)
    {
        approved = 0;
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "revision") == 0)
    {
        --revision;
        expected = UMI_STATUS_BUSY;
    }
    if (strcmp(mode, "stale-target") == 0)
    {
        CHECK(Draft(&second, "later") == UMI_STATUS_OK);
        before_second = "later";
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "stale-dependency") == 0 || strcmp(mode, "dependency-round-trip") == 0)
    {
        CHECK(Draft(&dependency, "later") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
        if (strcmp(mode, "dependency-round-trip") == 0)
            CHECK(Draft(&dependency, "dependency") == UMI_STATUS_OK);
    }
    if (strcmp(mode, "dependency-caret") == 0 || strcmp(mode, "dependency-permission") == 0)
    {
        if (strcmp(mode, "dependency-caret") == 0)
            dependency_view.cursor_offset = 1U;
        else
            dependency_view.read_only = 1;
        CHECK(SourceFixturePut(&dependency, &dependency_view) == 0);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "dependency-saved") == 0)
    {
        CHECK(umi_document_store_mark_saved_as(first.store, dependency.id, "dependency-saved.h") ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "dependency-closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(first.documents, 1) == UMI_STATUS_OK);
        expected = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "active-tab") == 0)
        CHECK(umi_ui_document_view_model_activate(umi_ui_workbench_documents(first.workbench),
                                                  first.viewId) == UMI_STATUS_OK);
    if (strcmp(mode, "new-document") == 0)
        CHECK(umi_document_coordinator_new(first.documents, "later.c", NULL, 0U) == UMI_STATUS_OK);
    UmiDocumentSnapshot stored_before, stored_after;
    CHECK(umi_document_store_snapshot(first.store, first.id, &stored_before) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active_before, active_after;
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &active_before) == UMI_STATUS_OK);
    uint64_t view_revision = umi_ui_document_view_model_revision(umi_ui_workbench_documents(first.workbench));
    CHECK(UmiSourceWorkspaceEditReviewApply(review, revision, approved) == expected);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(ExpectText(&first, before_first) == 0 && ExpectText(&second, before_second) == 0);
        CHECK(umi_document_store_snapshot(first.store, first.id, &stored_after) == UMI_STATUS_OK &&
              stored_after.revision == stored_before.revision);
        CHECK(umi_ui_document_view_model_revision(umi_ui_workbench_documents(first.workbench)) ==
              view_revision);
        goto cleanup;
    }
    const char *replacement = strcmp(mode, "unchanged") == 0 || strcmp(mode, "annotation-noop") == 0
                                  ? "source"
                              : strcmp(mode, "empty-replacement") == 0 ? ""
                              : strcmp(mode, "unicode") == 0           ? "caf\xc3\xa9"
                                                                       : "changed";
    char after_first[100], after_second[100];
    (void)snprintf(after_first, sizeof(after_first), "%s%s", replacement, before_first + 6U);
    (void)snprintf(after_second, sizeof(after_second), "%s%s", replacement, before_second + 6U);
    CHECK(ExpectText(&first, after_first) == 0 &&
          ExpectText(&second, documents == 1U ? before_second : after_second) == 0);
    CHECK(umi_document_coordinator_active_snapshot(first.documents, &active_after) == UMI_STATUS_OK &&
          active_after.document_id == active_before.document_id);
    CHECK(UmiSourceWorkspaceEditReviewInspect(review, &summary) == UMI_STATUS_OK && summary.applied &&
          summary.reviewed_count == summary.changed_count &&
          summary.confirmed_annotations == summary.required_annotations);
    if (strcmp(mode, "annotation-noop") == 0)
        CHECK(summary.changed_count == 0U && summary.required_annotations == 1U);
    if (strcmp(mode, "annotation-shared") == 0)
        CHECK(summary.required_annotations == 1U && summary.confirmed_annotations == 1U);
    if (strcmp(mode, "undo") == 0 || strcmp(mode, "redo") == 0 || strcmp(mode, "pending-typing") == 0)
    {
        CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK &&
              ExpectText(&first, before_first) == 0 && ExpectText(&second, after_second) == 0);
        CHECK(UmiDocumentCoordinatorUndo(first.documents, second.id) == UMI_STATUS_OK &&
              ExpectText(&second, before_second) == 0);
        if (strcmp(mode, "redo") == 0)
            CHECK(UmiDocumentCoordinatorRedo(first.documents, first.id) == UMI_STATUS_OK &&
                  ExpectText(&first, after_first) == 0);
        if (strcmp(mode, "pending-typing") == 0)
            CHECK(UmiDocumentCoordinatorUndo(first.documents, first.id) == UMI_STATUS_OK &&
                  ExpectText(&first, "source one") == 0);
    }
    if (strcmp(mode, "consumed") == 0)
    {
        CHECK(UmiSourceWorkspaceEditReviewApply(review, summary.revision, 1) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiSourceWorkspaceEditReviewAcceptDocument(review, 0U, summary.revision) ==
              UMI_STATUS_INVALID_STATE);
        const char *source = (const char *)1, *proposal = (const char *)1;
        size_t a = 1U, b = 1U;
        CHECK(UmiSourceWorkspaceEditReviewTexts(review, 0U, &source, &a, &proposal, &b) ==
                  UMI_STATUS_INVALID_STATE &&
              source == NULL && proposal == NULL && a == 0U && b == 0U);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        UmiSourceWorkspaceEditReview *bad = review;
        CHECK(UmiSourceWorkspaceEditReviewCreate(NULL, &sources, &catalogue, &version, NULL, &bad) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiSourceWorkspaceEditReviewInspect(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSourceWorkspaceEditReviewAt(review, 0U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSourceWorkspaceEditReviewAcceptDocument(review, documents, summary.revision) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(UmiSourceWorkspaceEditReviewCatalogue(NULL, &borrowed) == UMI_STATUS_INVALID_ARGUMENT &&
              borrowed == NULL);
        CHECK(UmiSourceWorkspaceEditReviewApply(NULL, 1U, 1) == UMI_STATUS_INVALID_ARGUMENT);
        UmiSourceWorkspaceEditReviewDestroy(NULL);
    }
cleanup:
    UmiSourceWorkspaceEditReviewDestroy(review);
    UmiDocumentSourceWorkspaceDestroy(sources);
    UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    Stop(&first);
    return 0;
}
