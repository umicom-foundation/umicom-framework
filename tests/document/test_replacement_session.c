/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_session.c
 * PURPOSE: Exercise captured multi-document decisions through the real document owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"

/* Each added draft has its own history. Moving the fixture's current ID only
 * changes which view its helpers inspect; it does not redirect the session. */
static int Add(ReplacementFixture *f, const char *text)
{
    CHECK(umi_document_coordinator_new(f->documents, "another.c", f->viewId, sizeof f->viewId) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
    f->id = active.document_id;
    CHECK(Draft(f, text) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(f->documents) == UMI_STATUS_OK);
    return 0;
}

static int Progress(UmiDocumentReplacementSession *session, UmiDocumentReplacementProgress *progress)
{
    CHECK(UmiDocumentReplacementSessionProgress(session, progress) == UMI_STATUS_OK);
    CHECK(progress->total == progress->applied + progress->skipped + progress->unchanged +
        progress->read_only + progress->closed + progress->remaining);
    return 0;
}

/* A two-document sequence makes partial completion observable. None of these
 * operations may save files, select another tab, or roll back an earlier Apply. */
static int Run(ReplacementFixture *f, UmiDocumentReplacementSession **owned, const char *name)
{
    UmiDocumentId first = f->id;
    char first_view[UMI_UI_ID_CAPACITY]; strcpy(first_view, f->viewId);
    UmiDocumentReplacementProgress progress;
    if (strcmp(name, "empty") == 0) CHECK(UmiDocumentCoordinatorClose(f->documents, first, 1) == UMI_STATUS_OK);
    else CHECK(Add(f, "note tail") == 0);
    UmiDocumentId second = f->id;
    char needle[] = "note", replacement[] = "saved";
    if (strcmp(name, "invalid") == 0) {
        CHECK(UmiDocumentReplacementSessionCreate(f->documents, "", replacement, owned) == UMI_STATUS_INVALID_ARGUMENT && *owned == NULL);
        CHECK(UmiDocumentReplacementSessionCreate(f->documents, "\xff", replacement, owned) == UMI_STATUS_INVALID_ARGUMENT && *owned == NULL);
        CHECK(UmiDocumentReplacementSessionCreate(NULL, needle, replacement, owned) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentReplacementSessionCreate(f->documents, needle, replacement, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    CHECK(UmiDocumentReplacementSessionCreate(f->documents,
        strcmp(name, "no-match") == 0 ? "absent" : needle,
        strcmp(name, "same") == 0 ? "note" : strcmp(name, "delete") == 0 ? "" : replacement, owned) == UMI_STATUS_OK);
    UmiDocumentReplacementSession *session = *owned;
    CHECK(Progress(session, &progress) == 0);
    if (strcmp(name, "empty") == 0) {
        CHECK(progress.total == 0U && progress.phase == UMI_DOCUMENT_REPLACEMENT_COMPLETE);
        CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
        CHECK(UmiDocumentReplacementSessionCancel(session) == UMI_STATUS_OK);
        CHECK(Progress(session, &progress) == 0 && progress.phase == UMI_DOCUMENT_REPLACEMENT_COMPLETE);
        return 0;
    }
    CHECK(progress.total == 2U && progress.remaining == 2U);
    if (strcmp(name, "owned-inputs") == 0) { needle[0] = 'x'; replacement[0] = 'x'; }
    if (strcmp(name, "late-open") == 0) CHECK(Add(f, "note later") == 0);
    if (strcmp(name, "closed") == 0) CHECK(UmiDocumentCoordinatorClose(f->documents, first, 1) == UMI_STATUS_OK);
    if (strcmp(name, "read-only") == 0 || strcmp(name, "pinned") == 0) {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), first_view, &view) == UMI_STATUS_OK);
        if (strcmp(name, "read-only") == 0) view.read_only = 1; else view.pinned = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &view) == UMI_STATUS_OK);
    }
    if (strcmp(name, "cancel-ready") == 0) {
        CHECK(UmiDocumentReplacementSessionCancel(session) == UMI_STATUS_OK);
        CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_CANCELLED);
        CHECK(Progress(session, &progress) == 0 && progress.remaining == 2U && progress.applied == 0U);
        return 0;
    }
    if (strcmp(name, "path-before-turn") == 0) {
        CHECK(umi_document_store_mark_saved_as(f->store, first, "different-review-target.c") == UMI_STATUS_OK);
        CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_INVALID_STATE);
        CHECK(Progress(session, &progress) == 0 && progress.phase == UMI_DOCUMENT_REPLACEMENT_FAILED && progress.remaining == 2U);
        return 0;
    }
    CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
    CHECK(Progress(session, &progress) == 0);
    if (strcmp(name, "no-match") == 0 || strcmp(name, "same") == 0) {
        CHECK(progress.unchanged == 1U && progress.phase == UMI_DOCUMENT_REPLACEMENT_READY);
        CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
        CHECK(Progress(session, &progress) == 0 && progress.unchanged == 2U && progress.remaining == 0U);
        return 0;
    }
    if (strcmp(name, "closed") == 0 || strcmp(name, "read-only") == 0) {
        CHECK(progress.closed + progress.read_only == 1U && progress.remaining == 1U);
        CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
        CHECK(Progress(session, &progress) == 0 && progress.current.document_id == second);
        CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_SKIP) == UMI_STATUS_OK);
        CHECK(Progress(session, &progress) == 0 && progress.skipped == 1U && progress.remaining == 0U);
        return 0;
    }
    CHECK(progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW && progress.current.document_id == first);
    CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
    CHECK(UmiDocumentReplacementSessionCheck(session) == UMI_STATUS_OK);
    const char *before = NULL, *after = NULL; size_t before_bytes = 0U, after_bytes = 0U;
    CHECK(UmiDocumentReplacementSessionTexts(session, &before, &before_bytes, &after, &after_bytes) == UMI_STATUS_OK);
    CHECK(before_bytes == 9U && strcmp(before, "note note") == 0);
    CHECK(strcmp(after, strcmp(name, "delete") == 0 ? " " : "saved saved") == 0 && after_bytes == strlen(after));
    if (strcmp(name, "path-after-capture") == 0 || strcmp(name, "close-question") == 0 || strcmp(name, "read-only-question") == 0) {
        UmiStatus expected = UMI_STATUS_INVALID_STATE;
        if (strcmp(name, "path-after-capture") == 0)
            CHECK(umi_document_store_mark_saved_as(f->store, first, "different-review-target.c") == UMI_STATUS_OK);
        else if (strcmp(name, "close-question") == 0) {
            CHECK(UmiDocumentCoordinatorClose(f->documents, first, 1) == UMI_STATUS_OK); expected = UMI_STATUS_NOT_FOUND;
        } else {
            UmiUiDocumentViewSnapshot view;
            CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), first_view, &view) == UMI_STATUS_OK);
            view.read_only = 1;
            CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &view) == UMI_STATUS_OK);
            expected = UMI_STATUS_PERMISSION_DENIED;
        }
        CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_APPLY) == expected);
        CHECK(Progress(session, &progress) == 0 && progress.phase == UMI_DOCUMENT_REPLACEMENT_FAILED && progress.applied == 0U);
        CHECK(ExpectText(f, "note tail") == 0); return 0;
    }
    if (strcmp(name, "invalid-decision") == 0) {
        CHECK(UmiDocumentReplacementSessionRespond(session, (UmiDocumentReplacementDecision)99) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentReplacementSessionCheck(session) == UMI_STATUS_OK);
        return 0;
    }
    if (strcmp(name, "stale") == 0 || strcmp(name, "skip-stale") == 0) {
        char saved_view[UMI_UI_ID_CAPACITY]; strcpy(saved_view, f->viewId); strcpy(f->viewId, first_view);
        CHECK(Draft(f, "later typing") == UMI_STATUS_OK); strcpy(f->viewId, saved_view);
        CHECK(UmiDocumentReplacementSessionCheck(session) == UMI_STATUS_INVALID_STATE);
        if (strcmp(name, "stale") == 0) {
            CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_APPLY) == UMI_STATUS_INVALID_STATE);
            CHECK(Progress(session, &progress) == 0 && progress.phase == UMI_DOCUMENT_REPLACEMENT_FAILED && progress.applied == 0U);
            CHECK(ExpectText(f, "note tail") == 0);
            CHECK(UmiDocumentReplacementSessionCancel(session) == UMI_STATUS_OK);
            CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_INVALID_STATE);
            return 0;
        }
    }
    int skip = strcmp(name, "skip") == 0 || strcmp(name, "skip-stale") == 0;
    CHECK(UmiDocumentReplacementSessionRespond(session, skip ? UMI_DOCUMENT_REPLACEMENT_SKIP : UMI_DOCUMENT_REPLACEMENT_APPLY) == UMI_STATUS_OK);
    CHECK(Progress(session, &progress) == 0 && progress.remaining == 1U);
    CHECK(progress.applied == (skip ? 0U : 1U) && progress.replacements == (skip ? 0U : 2U));
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK && active.document_id == f->id);
    if (strcmp(name, "undo") == 0) {
        CHECK(UmiDocumentCoordinatorUndo(f->documents, first) == UMI_STATUS_OK);
        strcpy(f->viewId, first_view); CHECK(ExpectText(f, "note note") == 0);
        CHECK(UmiDocumentCoordinatorRedo(f->documents, first) == UMI_STATUS_OK);
        CHECK(ExpectText(f, "saved saved") == 0); return 0;
    }
    if (strcmp(name, "cancel-partial") == 0) {
        CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_STOP) == UMI_STATUS_OK);
        CHECK(Progress(session, &progress) == 0 && progress.phase == UMI_DOCUMENT_REPLACEMENT_CANCELLED && progress.applied == 1U);
        CHECK(ExpectText(f, "note tail") == 0); strcpy(f->viewId, first_view);
        CHECK(ExpectText(f, "saved saved") == 0); return 0;
    }
    if (strcmp(name, "partial-failure") == 0) {
        CHECK(umi_document_store_mark_saved_as(f->store, second, "different-review-target.c") == UMI_STATUS_OK);
        CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_INVALID_STATE);
        CHECK(Progress(session, &progress) == 0 && progress.applied == 1U && progress.remaining == 1U);
        CHECK(ExpectText(f, "note tail") == 0); strcpy(f->viewId, first_view);
        CHECK(ExpectText(f, "saved saved") == 0); return 0;
    }
    if (strcmp(name, "fresh-turn") == 0) CHECK(Draft(f, "note changed before turn") == UMI_STATUS_OK);
    CHECK(UmiDocumentReplacementSessionStep(session) == UMI_STATUS_OK);
    CHECK(Progress(session, &progress) == 0 && progress.current.document_id == second);
    CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_APPLY) == UMI_STATUS_OK);
    CHECK(Progress(session, &progress) == 0 && progress.remaining == 0U && progress.phase == UMI_DOCUMENT_REPLACEMENT_COMPLETE);
    if (strcmp(name, "late-open") == 0) CHECK(ExpectText(f, "note later") == 0);
    else CHECK(ExpectText(f, strcmp(name, "fresh-turn") == 0 ? "saved changed before turn" : strcmp(name, "delete") == 0 ? " tail" : "saved tail") == 0);
    CHECK(UmiDocumentReplacementSessionRespond(session, UMI_DOCUMENT_REPLACEMENT_APPLY) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiDocumentReplacementSessionTexts(session, &before, &before_bytes, &after, &after_bytes) == UMI_STATUS_INVALID_STATE);
    CHECK(before == NULL && after == NULL && before_bytes == 0U && after_bytes == 0U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"apply", "skip", "empty", "invalid", "owned-inputs", "no-match", "same", "delete",
        "late-open", "closed", "read-only", "pinned", "cancel-ready", "invalid-decision", "stale", "skip-stale",
        "undo", "cancel-partial", "fresh-turn", "path-before-turn", "path-after-capture", "close-question", "read-only-question", "partial-failure"};
    int known = 0;
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index) if (strcmp(argv[1], cases[index]) == 0) known = 1;
    if (!known) return 2;
    ReplacementFixture f = {0}; UmiDocumentReplacementSession *session = NULL;
    int result = Start(&f, "note note");
    if (result == 0) result = Run(&f, &session, argv[1]);
    /* Session destruction must not dereference its former document owner. */
    Stop(&f); UmiDocumentReplacementSessionDestroy(session); return result;
}
