/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_panel.c
 * PURPOSE: Exercise native assignment, drag movement, stale controls and deliberate website publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban.h"
#include "umicom/kanban/developers.h"
#include "umicom/kanban/view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); exit(1); } } while (0)
static GtkWidget *Named(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Named(child, name); if (found != NULL) return found;
    }
    return NULL;
}
static GtkWidget *Button(GtkWidget *root, const char *label)
{
    if (GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), label) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Button(child, label); if (found != NULL) return found;
    }
    return NULL;
}
static void Click(GtkWidget *root, const char *label)
{ GtkWidget *button = Button(root, label); REQUIRE(button != NULL); g_signal_emit_by_name(button, "clicked"); }
static void Set(GtkWidget *root, const char *name, const char *text)
{ GtkWidget *entry = Named(root, name); REQUIRE(GTK_IS_EDITABLE(entry)); gtk_editable_set_text(GTK_EDITABLE(entry), text); }
static GObject *Controller(GtkWidget *widget, GType type)
{
    REQUIRE(widget != NULL); GListModel *controllers = gtk_widget_observe_controllers(widget);
    GObject *found = NULL;
    for (guint i = 0U; i < g_list_model_get_n_items(controllers); ++i) {
        GObject *object = g_list_model_get_item(controllers, i);
        if (G_TYPE_CHECK_INSTANCE_TYPE(object, type)) { found = object; break; }
        g_object_unref(object);
    }
    g_object_unref(controllers); REQUIRE(found != NULL); return found;
}
typedef struct FeedbackReentry { GtkWidget *panel; unsigned attempts; } FeedbackReentry;
static void ReenterFeedback(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; FeedbackReentry *entry = data;
    ++entry->attempts; Click(entry->panel, "Add card");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    if (!gtk_init_check()) { fprintf(stderr, "GTK display unavailable; native interaction coverage skipped.\n"); return 77; }
    UmiKanbanBoard *board = NULL;
    REQUIRE(umi_kanban_board_create_workflow("private", "PRIVATE_BOARD", &board) == UMI_STATUS_OK);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel != NULL); g_object_ref_sink(panel);
    Set(panel, "kanban-title", "PRIVATE_CARD_TITLE"); Click(panel, "Add card");
    REQUIRE(umi_kanban_board_count(board) == 1U);
    UmiKanbanCardSnapshot row; size_t count = 0U, total = 0U;
    REQUIRE(umi_kanban_board_page(board, umi_kanban_board_revision(board), NULL, 0U, &row, 1U, &count, &total) == UMI_STATUS_OK);
    REQUIRE(count == 1U && total == 1U);
    char id[UMI_KANBAN_ID_SIZE]; strcpy(id, row.card.id);
    if (strcmp(argv[1], "assignment") == 0) {
        Set(panel, "kanban-developer", "Zo\xc3\xab Morgan"); Click(panel, "Assign name");
        REQUIRE(umi_kanban_board_developer_count(board) == 1U);
        UmiKanbanDeveloper developer;
        REQUIRE(umi_kanban_board_developer_at(board, umi_kanban_board_revision(board), 0U, &developer) == UMI_STATUS_OK);
        REQUIRE(strcmp(developer.name, "Zo\xc3\xab Morgan") == 0);
        REQUIRE(umi_kanban_board_find(board, id, &row) == UMI_STATUS_OK);
        REQUIRE(strcmp(row.card.assignee, developer.id) == 0 && strcmp(row.card.title, "PRIVATE_CARD_TITLE") == 0);
        Set(panel, "kanban-developer", "zo\xc3\xab morgan"); Click(panel, "Assign name");
        REQUIRE(umi_kanban_board_developer_count(board) == 1U);
        Set(panel, "kanban-developer", ""); Click(panel, "Assign name");
        REQUIRE(umi_kanban_board_find(board, id, &row) == UMI_STATUS_OK && row.card.assignee[0] == '\0');
    } else if (strcmp(argv[1], "move") == 0) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Named(panel, "kanban-destination")), 3U);
        Click(panel, "Move selected");
        REQUIRE(umi_kanban_board_find(board, id, &row) == UMI_STATUS_OK && strcmp(row.column_id, "blocked") == 0);
    } else if (strcmp(argv[1], "drag") == 0) {
        GObject *drag = Controller(Named(panel, id), GTK_TYPE_DRAG_SOURCE);
        GdkContentProvider *provider = NULL;
        g_signal_emit_by_name(drag, "prepare", 0.0, 0.0, &provider); REQUIRE(provider != NULL);
        GValue value = G_VALUE_INIT; g_value_init(&value, G_TYPE_STRING);
        GError *error = NULL; REQUIRE(gdk_content_provider_get_value(provider, &value, &error)); REQUIRE(error == NULL);
        GObject *drop = Controller(Named(panel, "testing"), GTK_TYPE_DROP_TARGET);
        gboolean accepted = FALSE;
        GValue foreign = G_VALUE_INIT; g_value_init(&foreign, G_TYPE_STRING); g_value_set_string(&foreign, "foreign text");
        g_signal_emit_by_name(drop, "drop", &foreign, 0.0, 0.0, &accepted); REQUIRE(!accepted);
        g_signal_emit_by_name(drop, "drop", &value, 0.0, 0.0, &accepted); REQUIRE(accepted);
        REQUIRE(umi_kanban_board_find(board, id, &row) == UMI_STATUS_OK && strcmp(row.column_id, "testing") == 0);
        g_signal_emit_by_name(drag, "drag-end", NULL, TRUE);
        REQUIRE(umi_kanban_board_count(board) == 1U);
        g_value_unset(&value); g_value_unset(&foreign); g_object_unref(provider); g_object_unref(drop); g_object_unref(drag);
    } else if (strcmp(argv[1], "stale") == 0) {
        UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1); edit->kind = UMI_KANBAN_CREATE_CARD;
        strcpy(edit->card.id, "other"); strcpy(edit->card.title, "Other card"); strcpy(edit->column_id, "todo");
        REQUIRE(umi_kanban_board_apply(board, umi_kanban_board_revision(board), "external", edit, 1U, NULL) == UMI_STATUS_OK);
        uint64_t revision = umi_kanban_board_revision(board);
        Click(panel, "Add card"); REQUIRE(umi_kanban_board_count(board) == 2U && umi_kanban_board_revision(board) == revision);
        g_free(edit);
    } else if (strcmp(argv[1], "completion-reenter") == 0) {
        GtkWidget *status = Named(panel, "kanban-status"); REQUIRE(GTK_IS_LABEL(status));
        gtk_label_set_text(GTK_LABEL(status), "Before edit");
        FeedbackReentry entry = {panel, 0U};
        gulong handler = g_signal_connect(status, "notify::label", G_CALLBACK(ReenterFeedback), &entry);
        Click(panel, "Add card"); g_signal_handler_disconnect(status, handler);
        REQUIRE(entry.attempts == 1U && umi_kanban_board_count(board) == 2U);
    } else if (strcmp(argv[1], "stale-form") == 0) {
        UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1);
        REQUIRE(umi_kanban_board_find(board, id, &row) == UMI_STATUS_OK);
        edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = row.card;
        strcpy(edit->card.title, "Newer external title");
        REQUIRE(umi_kanban_board_apply(board, umi_kanban_board_revision(board), "external", edit, 1U, NULL) == UMI_STATUS_OK);
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
        Set(panel, "kanban-title", "Older user draft"); Click(panel, "Rename selected");
        REQUIRE(umi_kanban_board_find(board, id, &row) == UMI_STATUS_OK);
        REQUIRE(strcmp(row.card.title, "Newer external title") == 0); g_free(edit);
    } else if (strcmp(argv[1], "busy") == 0) {
        uint64_t revision = umi_kanban_board_revision(board); UmiKanbanGtk4SetBusy(panel, true);
        Click(panel, "Add card"); Click(panel, "Assign name");
        REQUIRE(umi_kanban_board_revision(board) == revision && UmiKanbanGtk4Refresh(panel) == UMI_STATUS_UNAVAILABLE);
        UmiKanbanGtk4SetBusy(panel, false); REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "retired") == 0) {
        GtkWidget *retained = Button(panel, "Add card"); REQUIRE(retained != NULL); g_object_ref(retained);
        UmiKanbanGtk4Retire(panel); umi_kanban_board_destroy(board); board = NULL;
        g_signal_emit_by_name(retained, "clicked"); REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_UNAVAILABLE);
        g_object_unref(panel); panel = NULL; g_signal_emit_by_name(retained, "clicked"); g_object_unref(retained);
    } else if (strcmp(argv[1], "publication") == 0) {
        /* GTK attaches an expander's content to its widget tree when expanded.
         * Open the public review form just as the user does before finding it. */
        GtkWidget *review = NULL;
        for (GtkWidget *child = gtk_widget_get_first_child(panel); child != NULL;
             child = gtk_widget_get_next_sibling(child))
            if (GTK_IS_EXPANDER(child)) { review = child; break; }
        REQUIRE(GTK_IS_EXPANDER(review));
        gtk_expander_set_expanded(GTK_EXPANDER(review), TRUE);
        Set(panel, "kanban-public-title", "Reviewed feature"); Click(panel, "Include reviewed card"); Click(panel, "Generate website");
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Named(panel, "kanban-public-source")));
        GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
        char *html = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
        REQUIRE(strstr(html, "Reviewed feature") != NULL && strstr(html, "PRIVATE_") == NULL && strstr(html, id) == NULL);
        g_free(html); Click(panel, "Clear selection"); REQUIRE(gtk_text_buffer_get_char_count(buffer) == 0);
    } else REQUIRE(0);
    if (panel != NULL) { UmiKanbanGtk4Retire(panel); g_object_unref(panel); }
    umi_kanban_board_destroy(board); return 0;
}
