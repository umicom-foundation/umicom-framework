/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_open_batch_capture.c
 *
 * PURPOSE:
 *   Verify bounded owned input, result inspection and pre-I/O rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Verify bounded owned input, result inspection and pre-I/O rejection. */
#include "open_batch_fixture.h"
static int run(OpenFixture *f, const char *name) {
  const char *paths[] = {BATCH_ROOT "a.c", BATCH_ROOT "b.c"};
  if (strcmp(name, "arguments") == 0) {
    CHECK(UmiDocumentOpenBatchCreate(NULL, 2U, &f->batch) == UMI_STATUS_INVALID_ARGUMENT &&
          f->batch == NULL);
    CHECK(UmiDocumentOpenBatchCreate(paths, 0U, &f->batch) == UMI_STATUS_INVALID_ARGUMENT &&
          f->batch == NULL);
    CHECK(UmiDocumentOpenBatchCreate(paths, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentOpenBatchCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentOpenBatchStep(NULL, f->coordinator, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  if (strcmp(name, "limit") == 0) {
    CHECK(UmiDocumentOpenBatchCreate(paths, UMI_DOCUMENT_OPEN_BATCH_MAXIMUM + 1U, &f->batch) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(f->batch == NULL && f->reads == 0U);
    return 0;
  }
  char owned[UMI_PATH_CAPACITY], long_path[UMI_PATH_CAPACITY + 1U];
  strcpy(owned, paths[0]);
  memset(long_path, 'x', sizeof long_path);
  long_path[sizeof long_path - 1U] = '\0';
  if (strcmp(name, "owned") == 0)
    paths[0] = owned;
  if (strcmp(name, "null-member") == 0)
    paths[0] = NULL;
  if (strcmp(name, "empty-member") == 0)
    paths[0] = "";
  if (strcmp(name, "relative") == 0)
    paths[0] = "relative.c";
  if (strcmp(name, "long-member") == 0)
    paths[0] = long_path;
  CHECK(UmiDocumentOpenBatchCreate(paths, 2U, &f->batch) == UMI_STATUS_OK);
  if (strcmp(name, "owned") == 0)
    strcpy(owned, "mutated");
  UmiDocumentOpenBatchSummary s;
  UmiDocumentOpenItem item;
  CHECK(UmiDocumentOpenBatchInspect(f->batch, &s) == UMI_STATUS_OK);
  CHECK(s.count == 2U && s.pending == 2U && s.processed == 0U && !s.busy && f->reads == 0U);
  CHECK(UmiDocumentOpenBatchAt(f->batch, 0U, &item) == UMI_STATUS_OK);
  CHECK(item.state == UMI_DOCUMENT_OPEN_PENDING && item.status == UMI_STATUS_UNAVAILABLE);
  if (strcmp(name, "inspect") == 0) {
    UmiDocumentOpenItem before;
    memcpy(&before, &item, sizeof item);
    CHECK(UmiDocumentOpenBatchAt(f->batch, 2U, &item) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&before, &item, sizeof item) == 0);
    CHECK(UmiDocumentOpenBatchInspect(f->batch, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentOpenBatchAt(NULL, 0U, &item) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  size_t index = 99U;
  CHECK(UmiDocumentOpenBatchStep(f->batch, f->coordinator, &index) == UMI_STATUS_OK && index == 0U);
  CHECK(UmiDocumentOpenBatchAt(f->batch, 0U, &item) == UMI_STATUS_OK);
  if (strcmp(name, "owned") == 0 || strcmp(name, "initial") == 0)
    CHECK(item.state == UMI_DOCUMENT_OPEN_SUCCEEDED && strcmp(item.path, BATCH_ROOT "a.c") == 0);
  else {
    CHECK(item.state == UMI_DOCUMENT_OPEN_FAILED && f->reads == 0U && item.view_id[0] == '\0');
    CHECK(item.status == (strcmp(name, "long-member") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED
                                                           : UMI_STATUS_INVALID_ARGUMENT));
  }
  CHECK(UmiDocumentOpenBatchStep(f->batch, f->coordinator, &index) == UMI_STATUS_OK && index == 1U);
  CHECK(UmiDocumentOpenBatchInspect(f->batch, &s) == UMI_STATUS_OK && s.pending == 0U &&
        s.processed == 2U);
  index = 99U;
  CHECK(UmiDocumentOpenBatchStep(f->batch, f->coordinator, &index) == UMI_STATUS_NOT_FOUND &&
        index == 99U);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *cases[] = {"arguments",   "limit",        "owned",    "initial",    "inspect",
                         "null-member", "empty-member", "relative", "long-member"};
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
