/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_open_batch_progress.c
 *
 * PURPOSE:
 *   Exercise partial success, preserved drafts, cancellation and callback reentry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise partial success, preserved drafts, cancellation and callback reentry. */
#include "open_batch_fixture.h"
static int run(OpenFixture *f, const char *name) {
  const char *paths[] = {BATCH_ROOT "a.c", BATCH_ROOT "b.c", BATCH_ROOT "c.c"};
  if (strcmp(name, "missing") == 0)
    paths[1] = BATCH_ROOT "missing.c";
  if (strcmp(name, "denied") == 0)
    paths[1] = BATCH_ROOT "denied.c";
  if (strcmp(name, "broken") == 0)
    paths[1] = BATCH_ROOT "broken.c";
  if (strcmp(name, "binary") == 0)
    paths[1] = BATCH_ROOT "binary.dat";
  if (strcmp(name, "huge") == 0)
    paths[1] = BATCH_ROOT "huge.c";
  if (strcmp(name, "duplicate") == 0 || strcmp(name, "dirty") == 0)
    paths[1] = paths[0];
  if (strcmp(name, "equivalent") == 0)
    paths[1] = BATCH_ROOT "folder/../a.c";
  if (strcmp(name, "all-failed") == 0)
    paths[0] = paths[1] = paths[2] = BATCH_ROOT "missing.c";
  CHECK(UmiDocumentOpenBatchCreate(paths, 3U, &f->batch) == UMI_STATUS_OK);
  UmiDocumentOpenBatchSummary s;
  size_t index = 99U;
  if (strcmp(name, "cancel-before") == 0) {
    CHECK(UmiDocumentOpenBatchCancel(f->batch) == UMI_STATUS_OK);
    CHECK(UmiDocumentOpenBatchInspect(f->batch, &s) == UMI_STATUS_OK && s.cancelled == 3U &&
          s.pending == 0U);
    CHECK(f->reads == 0U);
    return 0;
  }
  f->nested = strcmp(name, "reentry") == 0;
  CHECK(UmiDocumentOpenBatchStep(f->batch, f->coordinator, &index) == UMI_STATUS_OK && index == 0U);
  if (f->nested)
    CHECK(f->nested_step == UMI_STATUS_BUSY && f->nested_cancel == UMI_STATUS_BUSY);
  if (strcmp(name, "cancel-after") == 0) {
    CHECK(UmiDocumentOpenBatchCancel(f->batch) == UMI_STATUS_OK);
    CHECK(UmiDocumentOpenBatchCancel(f->batch) == UMI_STATUS_OK);
    CHECK(UmiDocumentOpenBatchInspect(f->batch, &s) == UMI_STATUS_OK && s.succeeded == 1U &&
          s.cancelled == 2U);
    CHECK(umi_document_coordinator_count(f->coordinator) == 1U);
    UmiDocumentOpenItem item;
    CHECK(UmiDocumentOpenBatchAt(f->batch, 1U, &item) == UMI_STATUS_OK &&
          item.status == UMI_STATUS_CANCELLED);
    return 0;
  }
  if (strcmp(name, "other-owner") == 0) {
    OpenFixture other = {0};
    CHECK(fixture_start(&other) == 0);
    UmiStatus status = UmiDocumentOpenBatchStep(f->batch, other.coordinator, &index);
    fixture_finish(&other);
    CHECK(status == UMI_STATUS_INVALID_STATE && index == 0U);
  }
  UmiDocumentOpenItem first;
  CHECK(UmiDocumentOpenBatchAt(f->batch, 0U, &first) == UMI_STATUS_OK);
  if (strcmp(name, "dirty") == 0) {
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), first.view_id,
                                          &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view,
                                           "unsaved\n", 8U) == UMI_STATUS_OK);
  }
  CHECK(UmiDocumentOpenBatchStep(f->batch, f->coordinator, &index) == UMI_STATUS_OK && index == 1U);
  if (strcmp(name, "dirty") == 0) {
    char *text = NULL;
    size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), first.view_id,
                                         &text, &length) == UMI_STATUS_OK);
    bool same = length == 8U && strcmp(text, "unsaved\n") == 0;
    UmiUiDocumentViewModelFreeText(text);
    CHECK(same);
  }
  CHECK(UmiDocumentOpenBatchStep(f->batch, f->coordinator, &index) == UMI_STATUS_OK && index == 2U);
  CHECK(UmiDocumentOpenBatchInspect(f->batch, &s) == UMI_STATUS_OK && s.pending == 0U &&
        s.processed == 3U);
  if (strcmp(name, "all-failed") == 0)
    CHECK(s.failed == 3U && s.succeeded == 0U &&
          umi_document_coordinator_count(f->coordinator) == 0U);
  else if (strcmp(name, "missing") == 0 || strcmp(name, "denied") == 0 ||
           strcmp(name, "broken") == 0 || strcmp(name, "binary") == 0 ||
           strcmp(name, "huge") == 0) {
    CHECK(s.failed == 1U && s.succeeded == 2U &&
          umi_document_coordinator_count(f->coordinator) == 2U);
    UmiDocumentOpenItem item;
    CHECK(UmiDocumentOpenBatchAt(f->batch, 1U, &item) == UMI_STATUS_OK);
    UmiStatus expected = strcmp(name, "missing") == 0  ? UMI_STATUS_NOT_FOUND
                         : strcmp(name, "denied") == 0 ? UMI_STATUS_PERMISSION_DENIED
                         : strcmp(name, "broken") == 0 ? UMI_STATUS_IO_ERROR
                         : strcmp(name, "huge") == 0   ? UMI_STATUS_CAPACITY_EXCEEDED
                                                       : UMI_STATUS_NOT_IMPLEMENTED;
    CHECK(item.status == expected);
  } else {
    CHECK(s.succeeded == 3U && s.failed == 0U);
    if (strcmp(name, "duplicate") == 0 || strcmp(name, "equivalent") == 0 ||
        strcmp(name, "dirty") == 0) {
      UmiDocumentOpenItem item;
      CHECK(UmiDocumentOpenBatchAt(f->batch, 1U, &item) == UMI_STATUS_OK);
      CHECK(strcmp(first.view_id, item.view_id) == 0 &&
            umi_document_coordinator_count(f->coordinator) == 2U && f->reads == 2U);
    } else
      CHECK(umi_document_coordinator_count(f->coordinator) == 3U);
  }
  CHECK(UmiDocumentOpenBatchCancel(f->batch) == UMI_STATUS_OK);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *cases[] = {"all",           "missing",      "denied",    "broken",     "binary",
                         "huge",          "all-failed",   "duplicate", "equivalent", "dirty",
                         "cancel-before", "cancel-after", "reentry",   "other-owner"};
  bool known = false;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  OpenFixture fixture = {0};
  int result = fixture_start(&fixture);
  if (!result)
    result = run(&fixture, argv[1]);
  fixture_finish(&fixture);
  return result;
}
