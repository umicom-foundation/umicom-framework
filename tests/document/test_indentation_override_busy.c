/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation_override_busy.c
 *
 * PURPOSE:
 *   Verify provider callbacks cannot mutate indentation during a save while harmless reads and
 * identical overrides remain allowed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/document.h"
#include "umicom/document/indentation_override.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
typedef struct Memory {
  char bytes[64];
  size_t length;
  unsigned calls;
  const char *mode;
  UmiDocumentCoordinator *documents;
  UmiDocumentIndentationReview review;
  UmiStatus observed;
} Memory;
static UmiStatus Read(void *data, const char *path, unsigned char **out, size_t *length) {
  (void)path;
  Memory *m = data;
  *out = malloc(m->length + 1U);
  if (!*out)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(*out, m->bytes, m->length);
  (*out)[m->length] = 0;
  *length = m->length;
  return UMI_STATUS_OK;
}
static UmiStatus Stat(void *data, const char *path, UmiDocumentFileInfo *out) {
  Memory *m = data;
  *out = (UmiDocumentFileInfo){0};
  snprintf(out->path, sizeof out->path, "%s", path);
  out->exists = out->regular_file = out->readable = out->writable = 1;
  out->byte_count = m->length;
  return UMI_STATUS_OK;
}
static UmiStatus Write(void *data, const char *path, const void *bytes, size_t length, int atomic) {
  (void)path;
  (void)atomic;
  Memory *m = data;
  UmiEditorIndentation two = {2U, 1};
  if (length > sizeof m->bytes)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (!strcmp(m->mode, "defaults-during-write"))
    m->observed = UmiDocumentCoordinatorSetIndentation(m->documents, &two);
  else if (!strcmp(m->mode, "inspect-during-write")) {
    UmiDocumentIndentationReview review;
    m->observed =
        UmiDocumentCoordinatorInspectIndentation(m->documents, m->review.document_id, &review);
  } else
    m->observed = UmiDocumentCoordinatorApplyIndentation(
        m->documents, &m->review, !strcmp(m->mode, "clear-during-write") ? NULL : &two, NULL);
  memcpy(m->bytes, bytes, length);
  m->length = length;
  ++m->calls;
  return UMI_STATUS_OK;
}
static void Release(void *data, void *bytes) {
  (void)data;
  free(bytes);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *names[] = {"set-during-write", "clear-during-write", "same-during-write",
                         "defaults-during-write", "inspect-during-write"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(argv[1], names[i]))
      known = 1;
  CHECK(known);
  Memory m = {.bytes = "a", .length = 1U, .mode = argv[1]};
  UmiDocumentProvider provider = {
      .struct_size = sizeof(UmiDocumentProvider),
      .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION,
      .provider_id = "indentation.memory",
      .scheme = "file",
      .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_WRITE |
               UMI_DOCUMENT_PROVIDER_STAT | UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE,
      .instance = &m,
      .read = Read,
      .write = Write,
      .stat = Stat,
      .release_bytes = Release};
  UmiCommandRegistry *commands = NULL;
  UmiUiWorkbench *workbench = NULL;
  UmiDocumentStore *store = NULL;
  CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
  CHECK(umi_ui_workbench_create("indentation.memory", commands, &workbench) == UMI_STATUS_OK);
  CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_create(store, workbench, &provider, &m.documents) ==
        UMI_STATUS_OK);
#ifdef _WIN32
  const char *path = "C:/indentation-memory/source.c";
#else
  const char *path = "/indentation-memory/source.c";
#endif
  char id[UMI_UI_ID_CAPACITY];
  UmiDocumentWorkingCopySnapshot active;
  CHECK(umi_document_coordinator_open(m.documents, path, id, sizeof id) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_active_snapshot(m.documents, &active) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorInspectIndentation(m.documents, active.document_id, &m.review) ==
        UMI_STATUS_OK);
  if (!strcmp(m.mode, "clear-during-write") || !strcmp(m.mode, "same-during-write")) {
    UmiEditorIndentation two = {2U, 1};
    CHECK(UmiDocumentCoordinatorApplyIndentation(m.documents, &m.review, &two, &m.review) ==
          UMI_STATUS_OK);
  }
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, id, &view) == UMI_STATUS_OK);
  view.dirty = 1;
  CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "changed", 7U) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_save_active(m.documents) == UMI_STATUS_OK && m.calls == 1U);
  CHECK(m.observed ==
        (!strcmp(m.mode, "same-during-write") || !strcmp(m.mode, "inspect-during-write")
             ? UMI_STATUS_OK
             : UMI_STATUS_BUSY));
  CHECK(UmiDocumentCoordinatorCheckIndentation(m.documents, &m.review) == UMI_STATUS_OK);
  CHECK(m.length == 7U && !memcmp(m.bytes, "changed", 7U));
  umi_document_coordinator_destroy(m.documents);
  umi_document_store_destroy(store);
  umi_ui_workbench_destroy(workbench);
  umi_command_registry_destroy(commands);
  return 0;
}
