/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bookmark_selection/test_selection.c
 *
 * PURPOSE:
 *   Verify selected bookmark navigation preserves drafts and rejects stale row decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *cases[] = {"visit",        "remove",   "stale-visit", "stale-remove",
                         "bounds",       "null",     "closed",      "same-name-reopen",
                         "removed-line", "readonly", "history",     "metadata",
                         "first",        "middle",   "last",        "stale-after-remove"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  BookmarkFixture f;
  BookmarkCreate(&f);
  uint64_t revision = f.marks.revision;
  UmiDocumentWorkingCopySnapshot before = f.active;
  if (!strcmp(name, "null")) {
    CHECK(UmiDocumentCoordinatorVisitBookmark(NULL, 0U, revision) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorRemoveBookmark(NULL, 0U, revision) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "bounds")) {
    CHECK(UmiDocumentCoordinatorVisitBookmark(f.documents, SIZE_MAX, revision) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(UmiDocumentCoordinatorRemoveBookmark(f.documents, 1U, revision) == UMI_STATUS_NOT_FOUND);
  } else if (!strcmp(name, "stale-visit") || !strcmp(name, "stale-remove")) {
    BookmarkCaret(&f, 13U);
    CHECK(UmiDocumentCoordinatorToggleBookmark(f.documents, NULL) == UMI_STATUS_OK);
    UmiStatus status = !strcmp(name, "stale-visit")
                           ? UmiDocumentCoordinatorVisitBookmark(f.documents, 0U, revision)
                           : UmiDocumentCoordinatorRemoveBookmark(f.documents, 0U, revision);
    CHECK(status == UMI_STATUS_INVALID_STATE);
    CHECK(BookmarkView(&f).cursor_offset == 13U && f.marks.count == 2U);
  } else if (!strcmp(name, "closed") || !strcmp(name, "same-name-reopen")) {
    CHECK(UmiDocumentCoordinatorClose(f.documents, before.document_id, 1) == UMI_STATUS_OK);
    if (!strcmp(name, "same-name-reopen"))
      CHECK(umi_document_coordinator_new(f.documents, "notes.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorVisitBookmark(f.documents, 0U, revision) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiDocumentCoordinatorRemoveBookmark(f.documents, 0U, revision) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorBookmarks(f.documents, &f.marks) == UMI_STATUS_OK &&
          f.marks.count == 0U);
  } else if (!strcmp(name, "removed-line")) {
    BookmarkDraft(&f, "short");
    UmiDocumentNavigationHistorySnapshot old, now;
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &old) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorVisitBookmark(f.documents, 0U, revision) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &now) == UMI_STATUS_OK &&
          now.revision == old.revision);
    CHECK(BookmarkView(&f).cursor_offset == 0U);
  } else if (!strcmp(name, "first") || !strcmp(name, "middle") || !strcmp(name, "last")) {
    BookmarkCaret(&f, 0U);
    CHECK(UmiDocumentCoordinatorToggleBookmark(f.documents, NULL) == UMI_STATUS_OK);
    BookmarkCaret(&f, 13U);
    CHECK(UmiDocumentCoordinatorToggleBookmark(f.documents, NULL) == UMI_STATUS_OK);
    BookmarkObserve(&f);
    size_t index = !strcmp(name, "first") ? 0U : !strcmp(name, "middle") ? 1U : 2U;
    UmiDocumentBookmark old[3], row;
    for (size_t i = 0U; i < 3U; ++i)
      CHECK(UmiDocumentCoordinatorBookmarkAt(f.documents, i, &old[i]) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorRemoveBookmark(f.documents, index, f.marks.revision) ==
          UMI_STATUS_OK);
    for (size_t i = 0U; i < 2U; ++i) {
      CHECK(UmiDocumentCoordinatorBookmarkAt(f.documents, i, &row) == UMI_STATUS_OK);
      CHECK(row.line == old[i + (i >= index ? 1U : 0U)].line);
    }
    CHECK(UmiDocumentCoordinatorBookmarkAt(f.documents, 2U, &row) == UMI_STATUS_NOT_FOUND);
  } else if (!strcmp(name, "remove") || !strcmp(name, "metadata") ||
             !strcmp(name, "stale-after-remove")) {
    UmiDocumentNavigationHistorySnapshot old, now;
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &old) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorRemoveBookmark(f.documents, 0U, revision) == UMI_STATUS_OK);
    BookmarkObserve(&f);
    CHECK(f.marks.count == 0U && f.marks.revision == revision + 1U);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &now) == UMI_STATUS_OK &&
          now.revision == old.revision);
    UmiUiDocumentViewSnapshot view = BookmarkView(&f);
    CHECK(view.dirty && view.cursor_offset == 0U &&
          !strcmp(view.source_text, "first\nsecond\nthird\n"));
    CHECK(f.active.undo_count == before.undo_count && f.active.redo_count == before.redo_count);
    if (!strcmp(name, "stale-after-remove")) {
      CHECK(UmiDocumentCoordinatorRemoveBookmark(f.documents, 0U, revision) ==
            UMI_STATUS_INVALID_STATE);
      CHECK(UmiDocumentCoordinatorVisitBookmark(f.documents, 0U, revision) ==
            UMI_STATUS_INVALID_STATE);
    }
  } else {
    if (!strcmp(name, "readonly")) {
      UmiUiDocumentViewSnapshot view = BookmarkView(&f);
      view.read_only = 1;
      view.pinned = 1;
      CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f.workbench), &view) ==
            UMI_STATUS_OK);
    }
    CHECK(UmiDocumentCoordinatorVisitBookmark(f.documents, 0U, revision) == UMI_STATUS_OK);
    UmiUiDocumentViewSnapshot view = BookmarkView(&f);
    CHECK(view.cursor_offset == 7U && view.dirty &&
          !strcmp(view.source_text, "first\nsecond\nthird\n"));
    CHECK(f.active.undo_count == before.undo_count && f.active.redo_count == before.redo_count);
    CHECK(f.marks.revision == revision);
    if (!strcmp(name, "readonly"))
      CHECK(view.read_only && view.pinned);
    if (!strcmp(name, "history")) {
      UmiDocumentNavigationHistorySnapshot history;
      CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history) == UMI_STATUS_OK &&
            history.can_go_back);
      CHECK(UmiDocumentCoordinatorTravel(f.documents, -1, history.revision, NULL) == UMI_STATUS_OK);
      CHECK(BookmarkView(&f).cursor_offset == 0U);
    }
  }
  BookmarkDestroy(&f);
  return 0;
}
