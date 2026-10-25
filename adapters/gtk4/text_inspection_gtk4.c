/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/text_inspection_gtk4.c
 *
 * PURPOSE:
 *   Present Unicode counts and escaped codepoint locations as ordinary text with bounded retained
 * rows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_inspection.h"
static void InspectionCell(GtkWidget *grid, const char *text, int column, int row) {
  GtkWidget *label = gtk_label_new(text);
  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
  gtk_grid_attach(GTK_GRID(grid), label, column, row, 1, 1);
}
static void InspectionRow(GtkWidget *grid, const char *title, size_t whole, size_t selection,
                          int row) {
  gchar *a = g_strdup_printf("%zu", whole), *b = g_strdup_printf("%zu", selection);
  InspectionCell(grid, title, 0, row);
  InspectionCell(grid, a, 1, row);
  InspectionCell(grid, b, 2, row);
  g_free(a);
  g_free(b);
}
static const char *NoticeName(UmiEditorTextNoticeKind kind) {
  switch (kind) {
  case UMI_EDITOR_TEXT_BIDI_CONTROL:
    return "bidirectional control";
  case UMI_EDITOR_TEXT_ZERO_WIDTH:
    return "zero-width character";
  case UMI_EDITOR_TEXT_NONBREAKING_SPACE:
    return "nonbreaking space";
  case UMI_EDITOR_TEXT_CONTROL:
    return "control character";
  default:
    return "review character";
  }
}
UmiStatus UmiGtk4TextInspectionCreate(const UmiEditorTextInspection *inspection, GtkWidget **out) {
  if (out == NULL || *out != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorTextInspectionSummary s;
  UmiStatus status = UmiEditorTextInspectionSummaryRead(inspection, &s);
  if (status != UMI_STATUS_OK)
    return status;
  /* Build all source-independent text before publishing GTK objects. */
  GString *rows = g_string_new(NULL);
  for (size_t i = 0U; i < s.retained_notices; ++i) {
    UmiEditorTextNotice notice;
    status = UmiEditorTextInspectionNoticeAt(inspection, i, &notice);
    if (status != UMI_STATUS_OK) {
      g_string_free(rows, TRUE);
      return status;
    }
    g_string_append_printf(rows, "Line %zu, scalar column %zu; byte %zu; U+%04X — %s\n",
                           notice.line, notice.scalar_column, notice.byte_offset,
                           (unsigned)notice.codepoint, NoticeName(notice.kind));
  }
  if (s.notice_count == 0U)
    g_string_append(rows, "No characters from the inspection notice categories were found.");
  if (s.notices_truncated)
    g_string_append_printf(
        rows, "\nShowing the first %zu of %zu notices. Counts still cover the whole document.",
        s.retained_notices, s.notice_count);
  GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 24);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
  gtk_widget_set_name(grid, "text.inspection.metrics");
  InspectionCell(grid, "Measure", 0, 0);
  InspectionCell(grid, "Document", 1, 0);
  InspectionCell(grid, "Selection", 2, 0);
  int row = 1;
#define METRIC(label, field) InspectionRow(grid, label, s.document.field, s.selection.field, row++)
  METRIC("UTF-8 bytes", bytes);
  METRIC("Unicode scalars", scalars);
  METRIC("UTF-16 units", utf16_units);
  METRIC("Logical lines", lines);
  METRIC("ASCII-separated words", words);
  METRIC("Spaces", spaces);
  METRIC("Tabs", tabs);
  METRIC("Non-ASCII scalars", non_ascii);
  METRIC("LF line breaks", lf);
  METRIC("CRLF line breaks", crlf);
  METRIC("CR line breaks", cr);
  METRIC("Blank lines", blank_lines);
  METRIC("Trailing-whitespace lines", trailing_whitespace_lines);
  METRIC("Longest line (scalars)", longest_line_scalars);
#undef METRIC
  gtk_box_append(GTK_BOX(body), grid);
  GtkWidget *explanation = gtk_label_new(
      "Counts describe a captured draft, not the current file on disk. "
      "Scalars are not displayed glyphs. Empty text has one logical line. Notice locations use "
      "one-based "
      "scalar columns and zero-based byte offsets. These characters can be intentional.");
  gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
  gtk_box_append(GTK_BOX(body), explanation);
  GtkWidget *view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), TRUE);
  gtk_widget_set_name(view, "text.inspection.notices");
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), rows->str, -1);
  g_string_free(rows, TRUE);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 180);
  gtk_box_append(GTK_BOX(body), scroll);
  *out = body;
  return UMI_STATUS_OK;
}
