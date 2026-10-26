/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_card_labels.inc
 *
 * PURPOSE:
 *   Edit private labels without changing unrelated planning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


static bool LabelsLive(Panel *panel)
{
    return panel->board && !panel->busy && !panel->labels_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->labels_form);
}
static void LabelsUnmap(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (!root) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel) { panel->labels_interrupted = true; panel->labels_ready = false; }
    g_object_unref(root);
}
static void LoadLabels(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->labels_form)) goto done;
    panel->labels_interrupted = false; panel->applying = true;
    UmiKanbanCardLabelsSnapshot snapshot;
    UmiStatus status = UmiKanbanCardLabelsRead(panel->board, panel->selection_revision, panel->selected, &snapshot);
    if (status != UMI_STATUS_OK) {
        if (LabelsLive(panel))
            gtk_label_set_text(GTK_LABEL(panel->labels_status), "Select a current active card before loading labels. The previous draft is retained.");
        goto complete;
    }
    for (size_t i = 0U; i < snapshot.labels.count; ++i)
        if (!DetailsText(snapshot.labels.labels[i], UMI_KANBAN_ID_SIZE)) {
            gtk_label_set_text(GTK_LABEL(panel->labels_status), "These labels are not UTF-8. The previous draft is retained.");
            goto complete;
        }
    panel->labels_ready = false; panel->labels_snapshot = snapshot;
    for (size_t i = 0U; i < UMI_KANBAN_MAX_LABELS; ++i) {
        gtk_editable_set_text(GTK_EDITABLE(panel->labels_text[i]),
            i < snapshot.labels.count ? snapshot.labels.labels[i] : "");
        if (!LabelsLive(panel)) goto complete;
    }
    char message[192];
    g_snprintf(message, sizeof message, "Labels for %s at revision %" G_GUINT64_FORMAT
        ". Blank rows are omitted; labels are case-sensitive.", snapshot.card_id, (guint64)snapshot.board_revision);
    gtk_label_set_text(GTK_LABEL(panel->labels_status), message);
    panel->labels_ready = LabelsLive(panel);
complete:
    panel->applying = false;
done:
    g_object_unref(root);
}
static void ApplyLabels(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!LabelsLive(panel) || !panel->labels_ready || !gtk_widget_get_mapped(button) ||
        !gtk_widget_is_ancestor(button, panel->labels_form)) goto done;
    panel->applying = true;
    UmiKanbanCardLabels labels = {0}; bool valid = true;
    for (size_t i = 0U; i < UMI_KANBAN_MAX_LABELS; ++i) {
        char text[UMI_KANBAN_ID_SIZE];
        if (!CopyEntry(panel->labels_text[i], text, sizeof text, true)) { valid = false; break; }
        if (!text[0]) continue;
        memcpy(labels.labels[labels.count++], text, strlen(text) + 1U);
    }
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (valid) {
        if (strcmp(panel->selected, panel->labels_snapshot.card_id) ||
            panel->selection_revision != panel->labels_snapshot.board_revision)
            status = UMI_STATUS_INVALID_STATE;
        else status = UmiKanbanCardLabelsApply(panel->board, panel->labels_snapshot.board_revision,
            panel->labels_snapshot.card_id, "local-owner", &labels);
    }
    if (status == UMI_STATUS_OK) {
        panel->labels_ready = false;
        panel->selection_revision = umi_kanban_board_revision(panel->board);
        (void)UmiKanbanGtk4Refresh(root);
        if (LabelsLive(panel))
            gtk_label_set_text(GTK_LABEL(panel->labels_status), "Labels changed. Save the board through its host; load labels again before another edit.");
    } else if (LabelsLive(panel)) {
        char text[256];
        g_snprintf(text, sizeof text, "Labels refused: %s. Keep each label within 63 UTF-8 bytes, remove duplicates, and reload after board changes.", umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->labels_status), text);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreateLabelsForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); panel->labels_form = box;
    gtk_widget_set_name(box, "kanban-card-labels");
    Bind(box, "unmap", G_CALLBACK(LabelsUnmap), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("Select a card and load its private labels. Up to eight distinct labels are retained; blank rows are omitted. These labels are not automatically published."));
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load selected labels", G_CALLBACK(LoadLabels));
    Button(panel, actions, "Apply labels", G_CALLBACK(ApplyLabels));
    for (size_t i = 0U; i < UMI_KANBAN_MAX_LABELS; ++i) {
        char caption[32], name[48];
        g_snprintf(caption, sizeof caption, "Label %zu (optional)", i + 1U);
        g_snprintf(name, sizeof name, "kanban-label-text-%zu", i);
        panel->labels_text[i] = Entry(box, caption, 63);
        gtk_widget_set_name(panel->labels_text[i], name); g_object_ref(panel->labels_text[i]);
    }
    panel->labels_status = Label("No card labels loaded.");
    gtk_widget_set_name(panel->labels_status, "kanban-labels-status");
    gtk_box_append(GTK_BOX(box), panel->labels_status); g_object_ref(panel->labels_status);
    return box;
}
