/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desk_federation_gtk4.c
 *
 * PURPOSE:
 *   Compose inert workspace controls and bind their lifetime to the existing Desk root.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "desk_federation_internal.h"
#include <string.h>

DfGtk *DfGtkGet(GtkWidget *root)
{
    DfGtk *ui;
    if (root == NULL) return NULL;
    ui = g_object_get_data(G_OBJECT(root), DF_GTK_KEY);
    return ui != NULL && !ui->invalid && ui->runtime != NULL ? ui : NULL;
}
static void ReleaseServices(DfGtk *ui)
{
    UmiDeskFederationDestroy(ui->service); ui->service = NULL;
    umi_data_server_destroy(ui->server); ui->server = NULL;
}
static void FreeUi(gpointer data)
{
    DfGtk *ui = data;
    ReleaseServices(ui);
    g_clear_object(&ui->panel);
    g_free(ui);
}
void UmiDeskFederationGtkInvalidate(GtkWidget *root)
{
    DfGtk *ui;
    if (root == NULL) return;
    ui = g_object_get_data(G_OBJECT(root), DF_GTK_KEY);
    if (ui == NULL || ui->invalid) return;
    /* This runs before Desk's runtime is freed, not merely at finalisation.
     * Retained buttons can still emit signals; DfGtkGet then refuses them. */
    ui->invalid = true; ui->runtime = NULL; ui->previewToken = 0U;
    gtk_widget_set_sensitive(ui->panel, FALSE);
    ReleaseServices(ui);
}
static GtkWidget *Text(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    return label;
}
static GtkEntry *Entry(GtkBox *box, const char *caption, const char *initial)
{
    GtkWidget *entry = gtk_entry_new();
    gtk_box_append(box, Text(caption));
    gtk_editable_set_text(GTK_EDITABLE(entry), initial);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_box_append(box, entry);
    return GTK_ENTRY(entry);
}
static GtkButton *Button(GtkBox *box, const char *caption, const char *tooltip, GCallback callback, GtkWidget *root)
{
    GtkWidget *button = gtk_button_new_with_label(caption);
    gtk_widget_set_tooltip_text(button, tooltip);
    g_signal_connect_object(button, "clicked", callback, root, 0);
    gtk_box_append(box, button);
    return GTK_BUTTON(button);
}
static GtkTextBuffer *Viewer(GtkBox *box, int height)
{
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(scroll, -1, height);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(box, scroll);
    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
}
UmiStatus UmiDeskFederationGtkAttach(GtkWidget *root, UmiDeskRuntime *runtime)
{
    DfGtk *ui;
    GtkWidget *body, *scroll, *row, *controls, *activity;
    if (root == NULL || !GTK_IS_BOX(root) || runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (g_object_get_data(G_OBJECT(root), DF_GTK_KEY) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    ui = g_try_new0(DfGtk, 1);
    if (ui == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    ui->runtime = runtime;
    ui->panel = gtk_expander_new("Workspaces and session restore");
    g_object_ref_sink(ui->panel);
    body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(body, 12); gtk_widget_set_margin_end(body, 12);
    gtk_widget_set_margin_top(body, 10); gtk_widget_set_margin_bottom(body, 10);
    scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 360);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), body);
    gtk_expander_set_child(GTK_EXPANDER(ui->panel), scroll);
    gtk_box_append(GTK_BOX(body), Text("Save application groups, capture a running set, and review requests before opening it again. Documents and business data remain with their applications."));
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(body), row);
    (void)Button(GTK_BOX(row), "Open / reload saved workspaces", "Explicitly open user-local storage or reload the current saved entries. No application starts.", G_CALLBACK(DfGtkOpenClicked), root);
    ui->activityBadge = GTK_LABEL(Text("Desk activity: not opened"));
    gtk_box_append(GTK_BOX(row), GTK_WIDGET(ui->activityBadge));
    ui->storage = GTK_LABEL(Text("Workspace storage is closed. Opening this panel does not create a database."));
    gtk_label_set_selectable(ui->storage, TRUE);
    gtk_box_append(GTK_BOX(body), GTK_WIDGET(ui->storage));
    controls = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); ui->controls = controls;
    gtk_box_append(GTK_BOX(body), controls);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(controls), row);
    ui->id = Entry(GTK_BOX(row), "Saved ID", "finance");
    gtk_entry_set_max_length(ui->id, (gint)UMI_DESK_FEDERATION_WORKSPACE_ID_CAPACITY - 1);
    ui->title = Entry(GTK_BOX(row), "Display name", "Finance work");
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(controls), row);
    (void)Button(GTK_BOX(row), "Save selected as group", "Use the applications selected in Desk's existing picker. Reusing a group ID replaces that saved group.", G_CALLBACK(DfGtkSaveClicked), root);
    (void)Button(GTK_BOX(row), "Capture running session", "Save IDs of currently tracked running applications, excluding Desk. Unsaved documents are not captured.", G_CALLBACK(DfGtkCaptureClicked), root);
    ui->saved = GTK_DROP_DOWN(gtk_drop_down_new_from_strings((const char *const[]){"No saved workspaces", NULL}));
    gtk_box_append(GTK_BOX(controls), GTK_WIDGET(ui->saved));
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(controls), row);
    (void)Button(GTK_BOX(row), "Preview saved workspace", "Show which applications would start, switch or be skipped. Nothing is launched yet.", G_CALLBACK(DfGtkPreviewClicked), root);
    (void)Button(GTK_BOX(row), "Remove saved entry", "Remove only this saved application set. Does not close windows, stop processes or delete documents.", G_CALLBACK(DfGtkRemoveClicked), root);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(controls), row);
    ui->link = Entry(GTK_BOX(row), "Umicom link", "umicom://group/finance");
    gtk_entry_set_max_length(ui->link, (gint)UMI_DESK_FEDERATION_LINK_CAPACITY - 1);
    (void)Button(GTK_BOX(row), "Preview link", "Accepts app, group and session links only. No shell commands, file paths, external URLs or payment instructions.", G_CALLBACK(DfGtkLinkClicked), root);
    ui->preview = Viewer(GTK_BOX(controls), 130);
    gtk_text_buffer_set_text(ui->preview, "Choose a saved workspace or enter an Umicom link, then preview it.", -1);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(controls), row);
    ui->confirm = Button(GTK_BOX(row), "Confirm reviewed requests", "Dispatch this single-use review through Desk's existing launcher. A request may be accepted before a window is ready.", G_CALLBACK(DfGtkConfirmClicked), root);
    (void)Button(GTK_BOX(row), "Cancel preview", "Discard the review without touching any application.", G_CALLBACK(DfGtkCancelClicked), root);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->confirm), FALSE);
    activity = gtk_expander_new("Recent Desk activity (this run only)");
    row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    ui->activity = Viewer(GTK_BOX(row), 130);
    (void)Button(GTK_BOX(row), "Mark activity read", "Acknowledge the visible local activity list; it is not a durable business audit.", G_CALLBACK(DfGtkReadClicked), root);
    gtk_expander_set_child(GTK_EXPANDER(activity), row); gtk_box_append(GTK_BOX(controls), activity);
    ui->status = GTK_LABEL(Text("Open saved workspaces to begin."));
    gtk_label_set_selectable(ui->status, TRUE); gtk_box_append(GTK_BOX(body), GTK_WIDGET(ui->status));
    gtk_widget_set_sensitive(controls, FALSE);
    g_object_set_data_full(G_OBJECT(root), DF_GTK_KEY, ui, FreeUi);
    g_signal_connect_object(ui->saved, "notify::selected", G_CALLBACK(DfGtkSelected), root, 0);
    g_signal_connect_object(ui->id, "changed", G_CALLBACK(DfGtkInputChanged), root, 0);
    g_signal_connect_object(ui->title, "changed", G_CALLBACK(DfGtkInputChanged), root, 0);
    g_signal_connect_object(ui->link, "changed", G_CALLBACK(DfGtkInputChanged), root, 0);
    gtk_box_append(GTK_BOX(root), ui->panel);
    return UMI_STATUS_OK;
}
UmiStatus UmiDeskFederationGtkOpenStorage(GtkWidget *root, const char *absolutePath)
{
    DfGtk *ui = DfGtkGet(root);
    UmiDeskFederationPorts ports;
    UmiStatus status;
    char *directory = NULL, *path = NULL;
    if (ui == NULL) return UMI_STATUS_INVALID_STATE;
    if (absolutePath != NULL && !g_path_is_absolute(absolutePath)) return UMI_STATUS_INVALID_ARGUMENT;
    DfGtkCancel(ui);
    if (ui->service != NULL) {
        status = UmiDeskFederationReload(ui->service);
        gtk_widget_set_sensitive(ui->controls, status == UMI_STATUS_OK);
        if (status == UMI_STATUS_OK) DfGtkRefreshWorkspaces(ui);
        return status;
    }
    if (absolutePath != NULL) {
        if (!g_path_is_absolute(absolutePath)) return UMI_STATUS_INVALID_ARGUMENT;
        path = g_strdup(absolutePath);
    } else {
        const char *dataRoot = g_get_user_data_dir();
        if (dataRoot == NULL || !g_path_is_absolute(dataRoot)) return UMI_STATUS_INVALID_STATE;
        directory = g_build_filename(dataRoot, "umicom", "desk", NULL);
        if (g_mkdir_with_parents(directory, 0700) != 0) { g_free(directory); return UMI_STATUS_IO_ERROR; }
        path = g_build_filename(directory, "workspaces.sqlite", NULL);
        g_free(directory);
    }
    status = umi_data_server_create_sqlite(path, &ui->server);
    ports = UmiDeskFederationRuntimePorts(ui->runtime);
    if (status == UMI_STATUS_OK) status = UmiDeskFederationCreate(ui->server, &ports, "org.umicom.desktop", &ui->service);
    if (status == UMI_STATUS_OK) status = UmiDeskFederationReload(ui->service);
    if (status == UMI_STATUS_OK) {
        gtk_label_set_text(ui->storage, path);
        gtk_widget_set_sensitive(ui->controls, TRUE);
        DfGtkRefreshWorkspaces(ui);
        (void)UmiDeskFederationPoll(ui->service); /* Establish evidence, no invented opening events. */
        DfGtkRefreshActivity(ui);
    } else ReleaseServices(ui);
    g_free(path); return status;
}
UmiStatus UmiDeskFederationGtkPoll(GtkWidget *root)
{
    DfGtk *ui = DfGtkGet(root);
    UmiStatus status;
    if (ui == NULL) return UMI_STATUS_INVALID_STATE;
    if (ui->service == NULL) return UMI_STATUS_OK;
    status = UmiDeskFederationPoll(ui->service);
    if (status == UMI_STATUS_OK) DfGtkRefreshActivity(ui);
    return status;
}
