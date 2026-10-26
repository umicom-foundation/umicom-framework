/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_card_details.inc
 *
 * PURPOSE:
 *   Expose private card descriptions, priorities and blockers through checked board transactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* These controls borrow the board through Panel. Each callback acquires the
 * owner and retains it across observable GTK setters. Form publication and
 * feedback share the existing panel edit guard; a hidden form must be loaded
 * again before its old draft can authorize an edit. */
static bool DetailsLive(Panel *panel)
{
    return panel->board != NULL && !panel->busy && !panel->details_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->details_form);
}
static bool DetailsText(const char *text, size_t capacity)
{
    const char *end = memchr(text, '\0', capacity);
    return end != NULL && g_utf8_validate(text, (gssize)(end - text), NULL);
}
static void DetailsUnmap(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (root == NULL) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel != NULL) { panel->details_interrupted = true; panel->details_ready = false; }
    g_object_unref(root);
}
static void LoadDetails(GtkWidget *button, gpointer data)
{
    (void)button;
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (panel == NULL) return;
    if (!gtk_widget_get_mapped(panel->details_form)) goto done;
    panel->details_interrupted = false;
    UmiKanbanCardDetailSnapshot snapshot;
    UmiStatus status = UmiKanbanCardDetailsRead(panel->board, panel->selection_revision,
        panel->selected, &snapshot);
    panel->applying = true;
    /* A failed replacement retains the visible draft and its old token. It
     * cannot bypass the later current-revision and selected-card checks. */
    if (status != UMI_STATUS_OK) {
        char text[192]; g_snprintf(text, sizeof text,
            "Cannot load details: %s. Select a current active card and try again.", umi_status_text(status));
        if (DetailsLive(panel)) gtk_label_set_text(GTK_LABEL(panel->details_status), text);
        goto complete;
    }
    if (!DetailsText(snapshot.details.title, sizeof snapshot.details.title) ||
        !DetailsText(snapshot.details.description, sizeof snapshot.details.description) ||
        !DetailsText(snapshot.details.blocked_reason, sizeof snapshot.details.blocked_reason)) {
        gtk_label_set_text(GTK_LABEL(panel->details_status), "These card bytes are not UTF-8. The previous draft is retained.");
        goto complete;
    }
    panel->details_ready = false;
    panel->details_snapshot = snapshot;
    gtk_editable_set_text(GTK_EDITABLE(panel->details_fields[0]), snapshot.details.title);
    if (!DetailsLive(panel)) goto complete;
    gtk_text_buffer_set_text(panel->details_description, snapshot.details.description, -1);
    if (!DetailsLive(panel)) goto complete;
    gtk_editable_set_text(GTK_EDITABLE(panel->details_fields[1]), snapshot.details.blocked_reason);
    if (!DetailsLive(panel)) goto complete;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->details_priority), (guint)snapshot.details.priority);
    if (!DetailsLive(panel)) goto complete;
    char identity[192];
    g_snprintf(identity, sizeof identity, "Editing card %s at board revision %" G_GUINT64_FORMAT
        ". Changes are private until separately reviewed for publication.", snapshot.card_id, (guint64)snapshot.board_revision);
    gtk_label_set_text(GTK_LABEL(panel->details_status), identity);
    panel->details_ready = DetailsLive(panel);
complete:
    panel->applying = false;
done:
    g_object_unref(root);
}
static void ApplyDetails(GtkWidget *button, gpointer data)
{
    (void)button;
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (panel == NULL) return;
    if (!DetailsLive(panel) || !panel->details_ready) goto done;
    panel->applying = true;
    UmiKanbanCardDetails details = {0};
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(panel->details_description, &first, &last);
    char *description = gtk_text_buffer_get_text(panel->details_description, &first, &last, FALSE);
    size_t length = strlen(description);
    guint priority = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->details_priority));
    bool valid = CopyEntry(panel->details_fields[0], details.title, sizeof details.title, false) &&
        CopyEntry(panel->details_fields[1], details.blocked_reason, sizeof details.blocked_reason, true) &&
        length < sizeof details.description && g_utf8_validate(description, -1, NULL) &&
        priority <= (guint)UMI_KANBAN_URGENT;
    if (valid) { memcpy(details.description, description, length + 1U); details.priority = (UmiKanbanPriority)priority; }
    g_free(description);
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (valid) {
        if (strcmp(panel->selected, panel->details_snapshot.card_id) != 0 ||
            panel->selection_revision != panel->details_snapshot.board_revision)
            status = UMI_STATUS_INVALID_STATE;
        else status = UmiKanbanCardDetailsApply(panel->board, panel->details_snapshot.board_revision,
            panel->details_snapshot.card_id, "local-owner", &details);
    }
    if (status == UMI_STATUS_OK) {
        /* Consume the form before native refresh. A status observer cannot
         * replay a previous draft, even if it recursively invokes Apply. */
        panel->details_ready = false;
        panel->selection_revision = umi_kanban_board_revision(panel->board);
        (void)UmiKanbanGtk4Refresh(root);
        if (DetailsLive(panel))
            gtk_label_set_text(GTK_LABEL(panel->details_status), "Card details changed. Save the board through its host. Load selected card to edit again.");
    } else if (DetailsLive(panel)) {
        char message[256]; g_snprintf(message, sizeof message,
            "Details refused: %s. Keep this draft; reload after reviewing newer board changes. Title/blocker limits are 255 UTF-8 bytes; description is 2047.",
            umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->details_status), message);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreateDetailsForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    panel->details_form = box;
    gtk_widget_set_name(box, "kanban-card-details");
    Bind(box, "unmap", G_CALLBACK(DetailsUnmap), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("Select a card, load its details, then apply your draft. Saving the board is a separate action. Clearing a blocker does not move the card."));
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load selected card details", G_CALLBACK(LoadDetails));
    Button(panel, actions, "Apply card details", G_CALLBACK(ApplyDetails));
    panel->details_fields[0] = Entry(box, "Private card title", 255);
    gtk_widget_set_name(panel->details_fields[0], "kanban-details-title");
    g_object_ref(panel->details_fields[0]);
    gtk_box_append(GTK_BOX(box), Label("Private description (up to 2047 UTF-8 bytes)"));
    GtkWidget *text = gtk_text_view_new();
    gtk_widget_set_name(text, "kanban-details-description");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD_CHAR);
    panel->details_description = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)));
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 120);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), text);
    gtk_box_append(GTK_BOX(box), scroll);
    panel->details_fields[1] = Entry(box, "Blocker reason (empty means no blocker)", 255);
    gtk_widget_set_name(panel->details_fields[1], "kanban-details-blocker");
    g_object_ref(panel->details_fields[1]);
    const char *priorities[] = {"Low", "Normal", "High", "Urgent", NULL};
    panel->details_priority = umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(priorities));
    gtk_widget_set_name(panel->details_priority, "kanban-details-priority");
    gtk_box_append(GTK_BOX(box), panel->details_priority); g_object_ref(panel->details_priority);
    panel->details_status = Label("No card details loaded.");
    gtk_widget_set_name(panel->details_status, "kanban-details-status");
    gtk_box_append(GTK_BOX(box), panel->details_status); g_object_ref(panel->details_status);
    return box;
}
