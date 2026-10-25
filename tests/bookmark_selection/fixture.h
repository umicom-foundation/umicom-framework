/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bookmark_selection/fixture.h
 *
 * PURPOSE:
 *   Create private in-memory drafts for bookmark navigation and metadata regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BOOKMARK_SELECTION_FIXTURE_H
#define UMICOM_BOOKMARK_SELECTION_FIXTURE_H
#include "umicom/document/document.h"
#include "umicom/document/navigation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
typedef struct BookmarkFixture {
  UmiCommandRegistry *commands;
  UmiUiWorkbench *workbench;
  UmiDocumentStore *store;
  UmiDocumentCoordinator *documents;
  UmiDocumentWorkingCopySnapshot active;
  UmiDocumentBookmarksSnapshot marks;
} BookmarkFixture;
static inline void BookmarkObserve(BookmarkFixture *f) {
  CHECK(umi_document_coordinator_active_snapshot(f->documents, &f->active) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorBookmarks(f->documents, &f->marks) == UMI_STATUS_OK);
}
static inline UmiUiDocumentViewSnapshot BookmarkView(BookmarkFixture *f) {
  BookmarkObserve(f);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->active.view_id,
                                        &view) == UMI_STATUS_OK);
  return view;
}
static inline void BookmarkDraft(BookmarkFixture *f, const char *text) {
  UmiUiDocumentViewSnapshot view = BookmarkView(f);
  view.cursor_offset = 0U;
  view.selection_length = 0U;
  view.dirty = 1;
  CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view, text,
                                         strlen(text)) == UMI_STATUS_OK);
}
static inline void BookmarkCaret(BookmarkFixture *f, size_t offset) {
  UmiUiDocumentViewSnapshot view = BookmarkView(f);
  view.cursor_offset = offset;
  view.selection_length = 0U;
  CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &view) ==
        UMI_STATUS_OK);
}
static inline void BookmarkCreate(BookmarkFixture *f) {
  memset(f, 0, sizeof *f);
  CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
  CHECK(umi_ui_workbench_create("test.bookmark.selection", f->commands, &f->workbench) ==
        UMI_STATUS_OK);
  CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
  CHECK(umi_document_coordinator_create(f->store, f->workbench, NULL, &f->documents) ==
        UMI_STATUS_OK);
  CHECK(umi_document_coordinator_new(f->documents, "notes.c", NULL, 0U) == UMI_STATUS_OK);
  BookmarkDraft(f, "first\nsecond\nthird\n");
  BookmarkCaret(f, 7U);
  CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
  BookmarkCaret(f, 0U);
  BookmarkObserve(f);
}
static inline void BookmarkDestroy(BookmarkFixture *f) {
  umi_document_coordinator_destroy(f->documents);
  umi_document_store_destroy(f->store);
  umi_ui_workbench_destroy(f->workbench);
  umi_command_registry_destroy(f->commands);
}
#endif
