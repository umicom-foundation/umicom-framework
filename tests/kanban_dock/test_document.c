/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_dock/test_document.c
 *
 * PURPOSE:
 *   Keep saved and unsaved board content intact through host closure and native docking.
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
  CHECK(argc == 3);
  if (!gtk_init_check())
    return 77;
  Fixture f = {0};
  Open(&f);
  char *directory = g_build_filename(argv[2], "dock-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory) != NULL);
  Create(&f, directory);
  if (!strcmp(argv[1], "busy")) {
    Click(f.content, "Save board");
    CHECK(UmiKanbanGtk4WindowBusy(f.board));
    CHECK(UmiKanbanGtk4DockWindow(f.host, f.board, &f.pane) == UMI_STATUS_BUSY);
    Wait(&f);
  } else {
    Dock(&f);
    Add(&f, "Unsaved docked card");
    if (!strcmp(argv[1], "close-refuses")) {
      /* Docking leaves the board owner unrealized. GTK's window-manager close
       * convenience call ignores an unrealized window. Exercise its actual
       * close-request policy and assert that unsaved content prevents closure. */
#if 0
      gtk_window_close(f.board);
#endif
      gboolean handled = FALSE;
      g_signal_emit_by_name(f.board, "close-request", &handled);
      CHECK(handled);
      Wait(&f);
      CHECK(UmiKanbanGtk4WindowDocked(f.board));
      const char *text = gtk_label_get_text(GTK_LABEL(Find(f.content, "kanban-document-status")));
      CHECK(strstr(text, "unsaved changes") != NULL);
    } else if (!strcmp(argv[1], "host-preserves")) {
      gtk_window_destroy(f.host);
      CHECK(gtk_window_get_child(f.board) == f.content);
    } else if (!strcmp(argv[1], "return-preserves")) {
      OK(UmiKanbanGtk4Undock(f.pane, true));
      CHECK(Find(f.content, "kanban-title") != NULL);
    } else if (!strcmp(argv[1], "save-docked")) {
      Click(f.content, "Save board");
      Wait(&f);
      CHECK(UmiKanbanGtk4WindowDocked(f.board));
    } else if (!strcmp(argv[1], "save-then-return")) {
      Click(f.content, "Save board");
      OK(UmiKanbanGtk4Undock(f.pane, true));
      Wait(&f);
    } else if (!strcmp(argv[1], "host-close-while-saving")) {
      Click(f.content, "Save board");
      gtk_window_destroy(f.host);
      Wait(&f);
      CHECK(gtk_window_get_child(f.board) == f.content);
    } else if (!strcmp(argv[1], "retire-while-saving")) {
      Click(f.content, "Save board");
      gtk_window_destroy(f.board);
      Wait(&f);
      CHECK(UmiKanbanGtk4Undock(f.pane, true) == UMI_STATUS_CANCELLED);
      Close(&f);
      g_free(directory);
      return 0;
    } else
      return 2;
    /* Prove unsaved model ownership survived the native move by saving,
     * reopening the archive and reading the card through the domain API. */
    Click(f.content, "Save board");
    Wait(&f);
    Click(f.content, "Close board");
    Wait(&f);
    UmiKanbanDocument *saved = NULL;
    OK(umi_kanban_document_open(directory, "board.umikb", &saved));
    CHECK(umi_kanban_board_count(umi_kanban_document_board(saved)) == 1U);
    OK(umi_kanban_document_close(&saved, false));
  }
  Close(&f);
  g_free(directory);
  return 0;
}
