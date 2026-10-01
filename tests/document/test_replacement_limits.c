/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_limits.c
 * PURPOSE: Check full-text review, Unicode validation and capacity rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"

/* Exercise boundaries in owned buffers, never on a truncated view preview. */
static int Run(ReplacementFixture *f, const char *name)
{
    if (strcmp(name, "invalid-draft") == 0) {
        CHECK(Draft(f, "note\xff") == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", "saved", &f->plan) == UMI_STATUS_INVALID_STATE);
        CHECK(f->plan == NULL && ExpectText(f, "note\xff") == 0);
        return 0;
    }
    if (strcmp(name, "invalid-utf8") == 0) {
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "\xc0\xaf", "x", &f->plan) == UMI_STATUS_INVALID_ARGUMENT && f->plan == NULL);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", "\xed\xa0\x80", &f->plan) == UMI_STATUS_INVALID_ARGUMENT && f->plan == NULL);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "", "x", &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(NULL, f->id, "note", "x", &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, 0U, "note", "x", &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, NULL, "x", &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", NULL, &f->plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", "x", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareReplacement(f->documents, UINT64_MAX, "note", "x", &f->plan) == UMI_STATUS_NOT_FOUND);
        CHECK(ExpectText(f, "note") == 0); return 0;
    }
    size_t bytes = strcmp(name, "full-text") == 0 ? 131072U : UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES;
    if (strcmp(name, "input-limit") == 0) ++bytes;
    char *text = malloc(bytes + 1U); CHECK(text != NULL);
    memset(text, 'x', bytes); text[bytes] = '\0';
    if (strcmp(name, "input-limit") == 0) {
        UmiStatus status = UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", text, &f->plan);
        free(text); CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && f->plan == NULL);
        CHECK(ExpectText(f, "note") == 0); return 0;
    }
    memcpy(text + bytes - 4U, "note", 4U);
    UmiStatus status = Draft(f, text); free(text); CHECK(status == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot original, current;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &original) == UMI_STATUS_OK);
    const char *replacement = strcmp(name, "growth") == 0 ? "notes" : "last";
    status = UmiDocumentCoordinatorPrepareReplacement(f->documents, f->id, "note", replacement, &f->plan);
    if (strcmp(name, "growth") == 0) {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && f->plan == NULL);
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &current) == UMI_STATUS_OK);
        CHECK(current.text_length == bytes && current.revision == original.revision && current.undo_count == original.undo_count);
        return 0;
    }
    if (strcmp(name, "maximum") != 0 && strcmp(name, "full-text") != 0) return 2;
    CHECK(status == UMI_STATUS_OK);
    const char *a = NULL, *b = NULL; size_t x = 0U, y = 0U;
    CHECK(UmiDocumentReplacementPlanTexts(f->plan, &a, &x, &b, &y) == UMI_STATUS_OK);
    CHECK(x == bytes && y == bytes && memcmp(a + x - 4U, "note", 4U) == 0 && memcmp(b + y - 4U, "last", 4U) == 0);
    CHECK(memcmp(a, b, bytes - 4U) == 0);
    CHECK(UmiDocumentCoordinatorApplyReplacement(f->documents, f->plan, NULL) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorUndo(f->documents, f->id) == UMI_STATUS_OK);
    char *restored = NULL; size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), f->viewId, &restored, &length) == UMI_STATUS_OK);
    int valid = length == bytes && memcmp(restored + length - 4U, "note", 4U) == 0;
    UmiUiDocumentViewModelFreeText(restored); CHECK(valid);
    return 0;
}

/* Boundary cases run separately to keep memory bounded during qualification. */
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    ReplacementFixture f = {0}; int result = Start(&f, "note");
    if (result == 0) result = Run(&f, argv[1]);
    Stop(&f); return result;
}
