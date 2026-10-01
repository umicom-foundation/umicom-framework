/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_capture.c
 * PURPOSE: Verify exact immutable replacement previews and non-mutating preparation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"

/* Exercise literal behavior against explicit expected text, not a second
 * invocation of the implementation's matching algorithm. */
static int Run(ReplacementFixture *f, const char *name)
{
    const char *before = "note NOTE note", *needle = "note", *replacement = "saved", *after = "saved saved saved";
    size_t count = 3U;
    if (strcmp(name, "draft") == 0) {
        before = "note note unsaved"; after = "saved saved unsaved"; count = 2U;
    } else if (strcmp(name, "case") == 0) {
        needle = "NOTE"; after = "note saved note"; count = 1U;
    } else if (strcmp(name, "unicode") == 0) {
        before = "caf\xc3\xa9 CAF\xc3\x89 caf\xc3\xa9"; needle = "caf\xc3\xa9";
        replacement = "\xc2\xa3"; after = "\xc2\xa3 CAF\xc3\x89 \xc2\xa3"; count = 2U;
    } else if (strcmp(name, "overlap") == 0) {
        before = "aaaaa"; needle = "aa"; replacement = "b"; after = "bba"; count = 2U;
    } else if (strcmp(name, "delete") == 0) {
        replacement = ""; after = "  ";
    } else if (strcmp(name, "literal") == 0) {
        before = "[a] [a]"; needle = "[a]"; replacement = "$1\\n"; after = "$1\\n $1\\n"; count = 2U;
    } else if (strcmp(name, "no-match") == 0) {
        needle = "absent"; after = before; count = 0U;
    } else if (strcmp(name, "empty") == 0) {
        before = ""; after = ""; count = 0U;
    } else if (strcmp(name, "unchanged") == 0) {
        needle = "NOTE"; replacement = "NOTE"; after = before; count = 1U;
    } else if (strcmp(name, "owned-inputs") != 0 && strcmp(name, "invalid-output") != 0) return 2;
    CHECK(Draft(f, before) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot original, current;
    UmiDocumentSnapshot stored, latest;
    UmiUiDocumentTextInfo textInfo, latestText;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &original) == UMI_STATUS_OK);
    CHECK(umi_document_store_snapshot(f->store, f->id, &stored) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelTextInfo(umi_ui_workbench_documents(f->workbench), f->viewId, &textInfo) == UMI_STATUS_OK);
    char needleCopy[64], replacementCopy[64];
    (void)snprintf(needleCopy, sizeof needleCopy, "%s", needle);
    (void)snprintf(replacementCopy, sizeof replacementCopy, "%s", replacement);
    CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, needleCopy, replacementCopy, &f->plan) == UMI_STATUS_OK);
    memset(needleCopy, 'x', strlen(needleCopy)); memset(replacementCopy, 'y', strlen(replacementCopy));
    CHECK(ExpectPlan(f, before, after, count) == 0);
    CHECK(ExpectText(f, before) == 0);
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &current) == UMI_STATUS_OK);
    CHECK(umi_document_store_snapshot(f->store, f->id, &latest) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelTextInfo(umi_ui_workbench_documents(f->workbench), f->viewId, &latestText) == UMI_STATUS_OK);
    CHECK(current.undo_count == original.undo_count && current.redo_count == original.redo_count && current.dirty == original.dirty);
    CHECK(latest.revision == stored.revision && latest.saved_revision == stored.saved_revision && latest.length == stored.length);
    CHECK(latestText.text_revision == textInfo.text_revision);
    if (strcmp(name, "invalid-output") == 0) {
        const char *a = "sentinel", *b = "sentinel"; size_t x = 99U, y = 99U;
        CHECK(UmiDocumentReplacementPlanTexts(f->plan, &a, &x, &b, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(a == NULL && b == NULL && x == 0U);
        CHECK(UmiDocumentReplacementPlanTexts(NULL, &a, &x, &b, &y) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(a == NULL && b == NULL && x == 0U && y == 0U);
        CHECK(UmiDocumentReplacementPlanSummary(f->plan, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    /* Closing the owner cannot alter a review's owned text. Apply is not
     * permitted after owner destruction, but reading/destroying still is. */
    umi_document_coordinator_destroy(f->documents); f->documents = NULL;
    CHECK(ExpectPlan(f, before, after, count) == 0);
    return 0;
}

/* Each registered case gets an independent coordinator and history. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    ReplacementFixture f = {0};
    int result = Start(&f, "stored text");
    if (result == 0) result = Run(&f, argv[1]);
    Stop(&f);
    return result;
}
