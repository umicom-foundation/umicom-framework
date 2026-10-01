/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/replacement_fixture.h
 * PURPOSE: Share memory-only working-copy fixtures for replacement review regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_REPLACEMENT_FIXTURE_H
#define UMICOM_TEST_REPLACEMENT_FIXTURE_H
#include "umicom/document/document.h"
#include "umicom/document/edit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct ReplacementFixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentReplacementPlan *plan;
    UmiDocumentReplacementPlan *second;
    UmiDocumentId id;
    char viewId[UMI_UI_ID_CAPACITY];
} ReplacementFixture;

/* Draft publication follows the native editor path without saving a file. */
static inline UmiStatus Draft(ReplacementFixture *f, const char *text)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, f->viewId, &view);
    if (status != UMI_STATUS_OK) return status;
    view.dirty = 1;
    view.cursor_offset = 0U;
    view.selection_length = 0U;
    return UmiUiDocumentViewModelUpsertText(views, &view, text, strlen(text));
}

/* Prepare the real shared service graph; the fixture never writes to disk. */
static inline int Start(ReplacementFixture *f, const char *text)
{
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.replacement", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(f->store, f->workbench, NULL, &f->documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(f->documents, "review.c", f->viewId, sizeof f->viewId) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    f->id = snapshot.document_id;
    CHECK(Draft(f, text) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(f->documents) == UMI_STATUS_OK);
    return 0;
}

/* Release plans independently of success; they borrow no text from the view. */
static inline void Stop(ReplacementFixture *f)
{
    UmiDocumentReplacementPlanDestroy(f->plan);
    UmiDocumentReplacementPlanDestroy(f->second);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
}

/* Read the entire draft so acceptance never mistakes a preview for full text. */
static inline int ExpectText(ReplacementFixture *f, const char *expected)
{
    char *text = NULL;
    size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), f->viewId, &text, &length) == UMI_STATUS_OK);
    int same = length == strlen(expected) && memcmp(text, expected, length) == 0;
    UmiUiDocumentViewModelFreeText(text);
    CHECK(same);
    return 0;
}

/* Inspect a capture without committing or replacing it with fresh state. */
static inline int ExpectPlan(ReplacementFixture *f, const char *before, const char *after, size_t count)
{
    UmiDocumentReplacementSummary summary;
    const char *previous = NULL, *proposed = NULL;
    size_t previousBytes = 0U, proposedBytes = 0U;
    CHECK(UmiDocumentReplacementPlanSummary(f->plan, &summary) == UMI_STATUS_OK);
    CHECK(summary.document_id == f->id && summary.match_count == count);
    CHECK(summary.text_changes == (strcmp(before, after) != 0));
    CHECK(UmiDocumentReplacementPlanTexts(f->plan, &previous, &previousBytes, &proposed, &proposedBytes) == UMI_STATUS_OK);
    CHECK(previousBytes == strlen(before) && strcmp(previous, before) == 0);
    CHECK(proposedBytes == strlen(after) && strcmp(proposed, after) == 0);
    CHECK(summary.previous_bytes == previousBytes && summary.proposed_bytes == proposedBytes);
    return 0;
}
#endif
