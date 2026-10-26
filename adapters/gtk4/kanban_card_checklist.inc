/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_card_checklist.inc
 *
 * PURPOSE:
 *   Edit acceptance checklist rows through the shared revision-bound card service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


static bool ChecklistLive(Panel *panel)
{
    return panel->board && !panel->busy && !panel->checklist_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->checklist_form);
}
static void ChecklistUnmap(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (root == NULL) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel) { panel->checklist_interrupted = true; panel->checklist_ready = false; }
    g_object_unref(root);
}
static void LoadChecklist(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->checklist_form)) goto done;
    panel->checklist_interrupted = false;
    panel->applying = true;
    UmiKanbanCardChecklistSnapshot snapshot;
    UmiStatus status = UmiKanbanCardChecklistRead(panel->board, panel->selection_revision, panel->selected, &snapshot);
    if (status != UMI_STATUS_OK) {
        if (ChecklistLive(panel))
            gtk_label_set_text(GTK_LABEL(panel->checklist_status), "Select a current active card before loading its checklist. The earlier draft is retained.");
        goto complete;
    }
    for (size_t i = 0U; i < snapshot.checklist.count; ++i)
        if (!DetailsText(snapshot.checklist.checks[i].text, sizeof snapshot.checklist.checks[i].text)) {
            gtk_label_set_text(GTK_LABEL(panel->checklist_status), "Checklist text is not UTF-8. The earlier draft is retained.");
            goto complete;
        }
    panel->checklist_ready = false;
    panel->checklist_snapshot = snapshot;
    for (size_t i = 0U; i < UMI_KANBAN_MAX_CHECKS; ++i) {
        const UmiKanbanCheck *item = i < snapshot.checklist.count ? &snapshot.checklist.checks[i] : NULL;
        gtk_editable_set_text(GTK_EDITABLE(panel->checklist_text[i]), item ? item->text : "");
        if (!ChecklistLive(panel)) goto complete;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->checklist_complete[i]), item && item->complete);
        if (!ChecklistLive(panel)) goto complete;
    }
    char message[192];
    g_snprintf(message, sizeof message, "Checklist for %s at revision %" G_GUINT64_FORMAT
        ". Checkmarks record your acceptance review, not a test run.", snapshot.card_id, (guint64)snapshot.board_revision);
    gtk_label_set_text(GTK_LABEL(panel->checklist_status), message);
    panel->checklist_ready = ChecklistLive(panel);
complete:
    panel->applying = false;
done:
    g_object_unref(root);
}
static void ApplyChecklist(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!ChecklistLive(panel) || !panel->checklist_ready || !gtk_widget_get_mapped(button) ||
        !gtk_widget_is_ancestor(button, panel->checklist_form)) goto done;
    panel->applying = true;
    UmiKanbanCardChecklist checklist = {0};
    bool valid = true;
    for (size_t i = 0U; i < UMI_KANBAN_MAX_CHECKS; ++i) {
        char text[256];
        if (!CopyEntry(panel->checklist_text[i], text, sizeof text, true)) { valid = false; break; }
        if (text[0] == '\0') continue; /* Explicitly empty rows are omitted. */
        UmiKanbanCheck *item = &checklist.checks[checklist.count++];
        memcpy(item->text, text, strlen(text) + 1U);
        item->complete = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->checklist_complete[i])) ? 1 : 0;
    }
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (valid) {
        if (strcmp(panel->selected, panel->checklist_snapshot.card_id) ||
            panel->selection_revision != panel->checklist_snapshot.board_revision)
            status = UMI_STATUS_INVALID_STATE;
        else status = UmiKanbanCardChecklistApply(panel->board, panel->checklist_snapshot.board_revision,
            panel->checklist_snapshot.card_id, "local-owner", &checklist);
    }
    if (status == UMI_STATUS_OK) {
        panel->checklist_ready = false;
        panel->selection_revision = umi_kanban_board_revision(panel->board);
        (void)UmiKanbanGtk4Refresh(root);
        if (ChecklistLive(panel))
            gtk_label_set_text(GTK_LABEL(panel->checklist_status), "Checklist changed. Save the board through its host. Load the checklist again for another edit.");
    } else if (ChecklistLive(panel)) {
        char message[256];
        g_snprintf(message, sizeof message, "Checklist refused: %s. Review stale drafts before reloading. Each item allows 255 UTF-8 bytes. A Done card cannot retain an unchecked item.",
            umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->checklist_status), message);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreateChecklistForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    panel->checklist_form = box; gtk_widget_set_name(box, "kanban-card-checklist");
    Bind(box, "unmap", G_CALLBACK(ChecklistUnmap), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("Load the selected card first. Up to 16 acceptance items are kept in row order. Empty text removes that row when you Apply. A checkmark is your reviewed assertion, not recorded test evidence."));
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load selected checklist", G_CALLBACK(LoadChecklist));
    Button(panel, actions, "Apply checklist", G_CALLBACK(ApplyChecklist));
    GtkWidget *scroll = gtk_scrolled_window_new(), *rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_size_request(scroll, -1, 220); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), rows);
    gtk_box_append(GTK_BOX(box), scroll);
    for (size_t i = 0U; i < UMI_KANBAN_MAX_CHECKS; ++i) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(rows), row);
        char label[32], name[64]; g_snprintf(label, sizeof label, "Item %u", (unsigned)(i + 1U));
        panel->checklist_complete[i] = gtk_check_button_new_with_label(label);
        g_snprintf(name, sizeof name, "kanban-checklist-complete-%u", (unsigned)i);
        gtk_widget_set_name(panel->checklist_complete[i], name);
        gtk_box_append(GTK_BOX(row), panel->checklist_complete[i]); g_object_ref(panel->checklist_complete[i]);
        panel->checklist_text[i] = gtk_entry_new(); gtk_entry_set_max_length(GTK_ENTRY(panel->checklist_text[i]), 255);
        gtk_widget_set_hexpand(panel->checklist_text[i], TRUE);
        g_snprintf(name, sizeof name, "kanban-checklist-text-%u", (unsigned)i);
        gtk_widget_set_name(panel->checklist_text[i], name);
        gtk_box_append(GTK_BOX(row), panel->checklist_text[i]); g_object_ref(panel->checklist_text[i]);
    }
    panel->checklist_status = Label("No checklist loaded."); gtk_widget_set_name(panel->checklist_status, "kanban-checklist-status");
    gtk_box_append(GTK_BOX(box), panel->checklist_status); g_object_ref(panel->checklist_status);
    return box;
}
