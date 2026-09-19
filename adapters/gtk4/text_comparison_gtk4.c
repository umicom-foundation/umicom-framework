/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/text_comparison_gtk4.c
 *
 * PURPOSE:
 *   Render a shared read-only text comparison without embedding document or
 *   file-writing policy in GTK widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/text_comparison.h"
#include "umicom/developer_productivity/text_comparison.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <stdlib.h>

/* Child controls may be externally retained. Their signals are weakly bound
 * to the root; no callback can use this state after root finalisation. */
typedef struct TextComparisonView {
    UmiTextComparison *model;
    GtkWidget *left;
    GtkWidget *right;
    GtkWidget *position;
    GtkWidget *previous;
    GtkWidget *next;
    size_t row;
} TextComparisonView;

static void DestroyView(gpointer data)
{
    TextComparisonView *view = data;
    if (view == NULL) return;
    UmiTextComparisonDestroy(view->model);
    free(view);
}

static GtkWidget *TextPane(const char *text, size_t length, const char *label,
    const char *tag, GtkWidget **outView)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *heading = gtk_label_new(label);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *editor = gtk_text_view_new();
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(editor), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(editor), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor), GTK_WRAP_NONE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(editor), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(editor), 8);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor)), text, (int)length);
    gtk_widget_set_tooltip_text(editor, "Read-only snapshot. Select and copy text; changes are not applied here.");
    (void)umi_gtk4_automation_tag_widget(editor, tag);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), scroll);
    *outView = editor;
    return box;
}

static void SelectLine(GtkWidget *editor, size_t line)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
    GtkTextIter first, last;
    /* A missing side has no matching source line: clear its old selection. */
    if (line == 0U) {
        gtk_text_buffer_get_iter_at_mark(buffer, &first, gtk_text_buffer_get_insert(buffer));
        gtk_text_buffer_place_cursor(buffer, &first);
        return;
    }
    if (!gtk_text_buffer_get_iter_at_line(buffer, &first, (int)(line - 1U))) return;
    last = first;
    (void)gtk_text_iter_forward_line(&last);
    gtk_text_buffer_select_range(buffer, &first, &last);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(editor), &first, 0.15, TRUE, 0.0, 0.3);
}

static void UpdateNavigation(TextComparisonView *view)
{
    size_t nextRow;
    gtk_widget_set_sensitive(view->next,
        UmiTextComparisonNavigate(view->model, view->row, 1, &nextRow) == UMI_STATUS_OK);
    gtk_widget_set_sensitive(view->previous,
        view->row != SIZE_MAX &&
        UmiTextComparisonNavigate(view->model, view->row, -1, &nextRow) == UMI_STATUS_OK);
}

static void Navigate(GtkButton *button, gpointer root)
{
    TextComparisonView *view = g_object_get_data(G_OBJECT(root), "umicom-text-comparison");
    if (view == NULL) return;
    size_t row;
    int direction = GTK_WIDGET(button) == view->next ? 1 : -1;
    if (UmiTextComparisonNavigate(view->model, view->row, direction, &row) != UMI_STATUS_OK) return;
    UmiDeveloperDiffRow match;
    if (UmiTextComparisonRow(view->model, row, &match) != UMI_STATUS_OK) return;
    view->row = row;
    SelectLine(view->left, match.left_line);
    SelectLine(view->right, match.right_line);
    char position[160], left[32], right[32];
    if (match.left_line) (void)snprintf(left, sizeof left, "%zu", match.left_line);
    else (void)snprintf(left, sizeof left, "none");
    if (match.right_line) (void)snprintf(right, sizeof right, "%zu", match.right_line);
    else (void)snprintf(right, sizeof right, "none");
    (void)snprintf(position, sizeof position, "Comparison row %zu — left line %s; right line %s", row + 1U, left, right);
    gtk_label_set_text(GTK_LABEL(view->position), position);
    UpdateNavigation(view);
}

UmiStatus UmiGtk4TextComparisonCreate(const char *left, size_t leftLength,
    const char *right, size_t rightLength, const char *leftLabel,
    const char *rightLabel, GtkWidget **outWidget)
{
    if (outWidget == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWidget = NULL;
    if (left == NULL || right == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (leftLength > UMI_TEXT_COMPARISON_MAXIMUM_BYTES || rightLength > UMI_TEXT_COMPARISON_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!g_utf8_validate(left, (gssize)leftLength, NULL) || !g_utf8_validate(right, (gssize)rightLength, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    TextComparisonView *view = calloc(1U, sizeof *view);
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiTextComparisonCreate(left, leftLength, right, rightLength, &view->model);
    if (status != UMI_STATUS_OK) { free(view); return status; }
    view->row = SIZE_MAX;
    UmiTextComparisonSummary summary;
    (void)UmiTextComparisonGetSummary(view->model, &summary);
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *panes = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    char detail[512];
    if (summary.alignmentStatus != UMI_STATUS_OK)
        (void)snprintf(detail, sizeof detail,
            "%s Both full texts are shown. Line alignment exceeds its 512-line or 1,023-byte-per-line limit; difference navigation is unavailable.",
            summary.identicalBytes ? "Texts are identical." : "Texts differ.");
    else
        (void)snprintf(detail, sizeof detail,
            "%s %zu changed comparison rows. Left: %zu bytes; right: %zu bytes. Final newline — left: %s; right: %s.%s",
            summary.identicalBytes ? "Texts are identical." : "Texts differ.", summary.changes.changed_rows,
            summary.leftBytes, summary.rightBytes, summary.leftFinalNewline ? "yes" : "no",
            summary.rightFinalNewline ? "yes" : "no",
            !summary.identicalBytes && summary.changes.changed_rows == 0U
                ? " The line comparison hides a line-ending difference; zero changed rows does not mean identical text." : "");
    GtkWidget *label = gtk_label_new(detail);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    (void)umi_gtk4_automation_tag_widget(label, "umicom.comparison.summary");
    gtk_box_append(GTK_BOX(root), label);
    gtk_paned_set_start_child(GTK_PANED(panes), TextPane(left, leftLength,
        leftLabel != NULL ? leftLabel : "Left text", "umicom.comparison.left", &view->left));
    gtk_paned_set_end_child(GTK_PANED(panes), TextPane(right, rightLength,
        rightLabel != NULL ? rightLabel : "Right text", "umicom.comparison.right", &view->right));
    gtk_paned_set_resize_start_child(GTK_PANED(panes), TRUE);
    gtk_paned_set_resize_end_child(GTK_PANED(panes), TRUE);
    gtk_paned_set_shrink_start_child(GTK_PANED(panes), FALSE);
    gtk_paned_set_shrink_end_child(GTK_PANED(panes), FALSE);
    gtk_paned_set_position(GTK_PANED(panes), 480);
    gtk_widget_set_vexpand(panes, TRUE);
    gtk_widget_set_hexpand(panes, TRUE);
    gtk_box_append(GTK_BOX(root), panes);
    view->previous = gtk_button_new_with_label("Previous difference");
    view->next = gtk_button_new_with_label("Next difference");
    view->position = gtk_label_new("Choose Next difference, or read either complete text.");
    gtk_label_set_wrap(GTK_LABEL(view->position), TRUE);
    gtk_label_set_xalign(GTK_LABEL(view->position), 0.0F);
    gtk_widget_set_tooltip_text(view->previous, "Select the earlier changed line; stops at the beginning.");
    gtk_widget_set_tooltip_text(view->next, "Select the next changed line in the captured snapshots; stops at the end.");
    (void)umi_gtk4_automation_tag_widget(view->previous, "umicom.comparison.previous");
    (void)umi_gtk4_automation_tag_widget(view->next, "umicom.comparison.next");
    gtk_box_append(GTK_BOX(buttons), view->previous);
    gtk_box_append(GTK_BOX(buttons), view->next);
    gtk_box_append(GTK_BOX(root), buttons);
    gtk_box_append(GTK_BOX(root), view->position);
    g_object_set_data_full(G_OBJECT(root), "umicom-text-comparison", view, DestroyView);
    g_signal_connect_object(view->previous, "clicked", G_CALLBACK(Navigate), G_OBJECT(root), 0);
    g_signal_connect_object(view->next, "clicked", G_CALLBACK(Navigate), G_OBJECT(root), 0);
    UpdateNavigation(view);
    *outWidget = root;
    return UMI_STATUS_OK;
}
