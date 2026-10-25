/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/open_batch_fixture.h
 *
 * PURPOSE:
 *   A deterministic provider exercises real document ownership without disk I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A deterministic provider exercises real document ownership without disk I/O. */
#ifndef UMICOM_OPEN_BATCH_FIXTURE_H
#define UMICOM_OPEN_BATCH_FIXTURE_H
#include "umicom/document/document.h"
#include "umicom/document/open_batch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define BATCH_ROOT "C:/umicom-fixture/"
#else
#define BATCH_ROOT "/umicom-fixture/"
#endif
#define CHECK(c)                                                                                   \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
typedef struct OpenFixture {
  UmiCommandRegistry *commands;
  UmiUiWorkbench *workbench;
  UmiDocumentStore *store;
  UmiDocumentCoordinator *coordinator;
  UmiDocumentOpenBatch *batch;
  size_t reads;
  bool nested;
  UmiStatus nested_step, nested_cancel;
} OpenFixture;
static UmiStatus fixture_stat(void *data, const char *path, UmiDocumentFileInfo *out) {
  (void)data;
  if (strstr(path, "missing") != NULL)
    return UMI_STATUS_NOT_FOUND;
  if (strstr(path, "denied") != NULL)
    return UMI_STATUS_PERMISSION_DENIED;
  *out = (UmiDocumentFileInfo){
      .exists = 1, .regular_file = 1, .readable = 1, .writable = 1, .byte_count = 11U};
  if (strstr(path, "huge") != NULL)
    out->byte_count = UMI_DOCUMENT_COORDINATOR_MAXIMUM_FILE_BYTES + 1U;
  return UMI_STATUS_OK;
}
static UmiStatus fixture_read(void *data, const char *path, unsigned char **out, size_t *bytes) {
  OpenFixture *fixture = data;
  ++fixture->reads;
  if (fixture->nested) {
    size_t index = 999U;
    fixture->nested_step = UmiDocumentOpenBatchStep(fixture->batch, fixture->coordinator, &index);
    fixture->nested_cancel = UmiDocumentOpenBatchCancel(fixture->batch);
  }
  if (strstr(path, "broken") != NULL)
    return UMI_STATUS_IO_ERROR;
  const unsigned char binary[] = {0U, 1U, 2U, 3U};
  const char text[] = "int a = 1;\n";
  *bytes = strstr(path, "binary") != NULL ? sizeof binary : strlen(text);
  *out = malloc(*bytes);
  if (*out == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(*out, strstr(path, "binary") != NULL ? (const void *)binary : (const void *)text, *bytes);
  return UMI_STATUS_OK;
}
static void fixture_release(void *data, void *bytes) {
  (void)data;
  free(bytes);
}
static int fixture_start(OpenFixture *fixture) {
  CHECK(umi_command_registry_create(&fixture->commands) == UMI_STATUS_OK);
  CHECK(umi_ui_workbench_create("open-batch", fixture->commands, &fixture->workbench) ==
        UMI_STATUS_OK);
  CHECK(umi_document_store_create(&fixture->store) == UMI_STATUS_OK);
  UmiDocumentProvider provider = {.struct_size = sizeof provider,
                                  .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION,
                                  .provider_id = "open-batch",
                                  .scheme = "file",
                                  .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_STAT,
                                  .instance = fixture,
                                  .read = fixture_read,
                                  .stat = fixture_stat,
                                  .release_bytes = fixture_release};
  CHECK(umi_document_coordinator_create(fixture->store, fixture->workbench, &provider,
                                        &fixture->coordinator) == UMI_STATUS_OK);
  return 0;
}
static void fixture_finish(OpenFixture *fixture) {
  UmiDocumentOpenBatchDestroy(fixture->batch);
  umi_document_coordinator_destroy(fixture->coordinator);
  umi_document_store_destroy(fixture->store);
  umi_ui_workbench_destroy(fixture->workbench);
  umi_command_registry_destroy(fixture->commands);
}
#endif
