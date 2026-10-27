/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_card_archive.inc
 *
 * PURPOSE:
 *   Review reversible card archive changes without deleting private board records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* A copied picker binds IDs to one revision. Reviewing an action is separate
 * from choosing a row; neither operation edits the board or deletes a card. */
static bool ArchiveLive(Panel *panel)
{
    return panel->board && !panel->busy && !panel->archive_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->archive_form);
}
static void ArchiveInvalidate(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (!root) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel) { panel->archive_ready = false; panel->archive_interrupted = true; }
    g_object_unref(root);
}
static void ArchiveSelection(GObject *object, GParamSpec *property, gpointer data)
{ (void)property; ArchiveInvalidate(GTK_WIDGET(object), data); }
static void LoadArchiveCards(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->archive_form)) goto done;
    panel->applying = true; panel->archive_ready = false;
    panel->archive_interrupted = false;
    uint64_t revision = umi_kanban_board_revision(panel->board);
    UmiKanbanFilter filter = {0}; filter.archived = 1;
    size_t count = 0U, total = 0U;
    UmiStatus status = umi_kanban_board_page(panel->board, revision, &filter, 0U, NULL, 0U, &count, &total);
    UmiKanbanCardSnapshot *rows = NULL;
    char (*ids)[UMI_KANBAN_ID_SIZE] = NULL;
    if (status == UMI_STATUS_OK && total > UMI_KANBAN_MAX_CARDS) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && total > 0U) {
        rows = g_try_new0(UmiKanbanCardSnapshot, total);
        ids = g_try_malloc0_n(total, sizeof *ids);
        status = rows && ids ? umi_kanban_board_page(panel->board, revision, &filter, 0U, rows, total, &count, &total)
            : UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) {
        for (size_t i = 0U; i < count; ++i) g_strlcpy(ids[i], rows[i].card.id, sizeof ids[i]);
        /* Own the complete list before changing any observable native property.
         * The string model is retained locally until the dropdown takes a ref. */
        GtkStringList *names = gtk_string_list_new(NULL);
        /* An explicit prompt is a real model row but never a card identity.
         * This keeps review admission independent of whether the installed
         * GTK dropdown can clear its selection while its model is nonempty. */
        gtk_string_list_append(names, "Choose a card");
        for (size_t i = 0U; i < count; ++i) {
            char *label = g_strdup_printf("%s | %s | %s", rows[i].archived ? "Archived" : "Active",
                rows[i].card.id, g_utf8_validate(rows[i].card.title, -1, NULL) ? rows[i].card.title : "Unsupported text encoding");
            gtk_string_list_append(names, label); g_free(label);
        }
        g_free(panel->archive_ids); panel->archive_ids = ids; ids = NULL;
        panel->archive_count = count; panel->archive_revision = revision;
        gtk_drop_down_set_model(GTK_DROP_DOWN(panel->archive_card), G_LIST_MODEL(names)); g_object_unref(names);
        /* Model replacement may invalidate a review. That is intentional. A
         * subsequent explicit Review is the only way to authorize an edit. */
        if (panel->board && !panel->busy && gtk_widget_get_mapped(panel->archive_form)) {
            /* The old empty-selection request is retained for comparison.
             * The prompt row replaces it without silently choosing a card. */
#if 0
            gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->archive_card), GTK_INVALID_LIST_POSITION);
#endif
            gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->archive_card), 0U);
            gtk_label_set_text(GTK_LABEL(panel->archive_status),
                count ? "Choose a card, then review its archive or restore action. No card has changed." : "This board has no cards to archive or restore.");
        }
    } else if (ArchiveLive(panel)) gtk_label_set_text(GTK_LABEL(panel->archive_status), "Card list could not be loaded. No card has changed.");
    g_free(ids); g_free(rows); panel->applying = false;
done:
    g_object_unref(root);
}
static void ReviewArchiveCard(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->archive_form)) goto done;
    panel->applying = true; panel->archive_ready = false; panel->archive_interrupted = false;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->archive_card));
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    UmiKanbanCardSnapshot row;
    /* Row zero is the prompt. Preserve the original direct mapping for review;
     * real card rows now start at one and map to the copied IDs at row minus one. */
#if 0
    if (selected < panel->archive_count && umi_kanban_board_revision(panel->board) == panel->archive_revision)
        status = umi_kanban_board_find(panel->board, panel->archive_ids[selected], &row);
#endif
    if (selected > 0U && (size_t)selected <= panel->archive_count &&
        umi_kanban_board_revision(panel->board) == panel->archive_revision)
        status = umi_kanban_board_find(panel->board, panel->archive_ids[selected - 1U], &row);
    if (status == UMI_STATUS_OK) {
        panel->archive_snapshot = row;
        char message[640];
        g_snprintf(message, sizeof message, "%s card %s in list %s at revision %" G_GUINT64_FORMAT
            ". Its fields and saved position are retained. %s",
            row.archived ? "Restore" : "Archive", row.card.id, row.column_id, (guint64)row.board_revision,
            row.archived ? "Restoring must satisfy the list limit and dependency rules." :
                "Archiving removes it from active lists. Active dependents can prevent archiving.");
        gtk_label_set_text(GTK_LABEL(panel->archive_status), message);
        panel->archive_ready = ArchiveLive(panel) &&
            gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->archive_card)) == selected;
    } else if (ArchiveLive(panel)) gtk_label_set_text(GTK_LABEL(panel->archive_status),
        "Reload cards and select a current row before reviewing. No card has changed.");
    panel->applying = false;
done:
    g_object_unref(root);
}
static void ApplyArchiveCard(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!ArchiveLive(panel) || !panel->archive_ready || !gtk_widget_get_mapped(button) ||
        !gtk_widget_is_ancestor(button, panel->archive_form)) goto done;
    panel->applying = true;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->archive_card));
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    /* Apply uses the same prompt-aware identity mapping as Review. The former
     * condition is retained so the offset change can be inspected. */
#if 0
    if (selected < panel->archive_count && !strcmp(panel->archive_ids[selected], panel->archive_snapshot.card.id)) {
#else
    if (selected > 0U && (size_t)selected <= panel->archive_count &&
        !strcmp(panel->archive_ids[selected - 1U], panel->archive_snapshot.card.id)) {
#endif
        UmiKanbanEdit edit = {0}; UmiKanbanProblem problem;
        edit.kind = panel->archive_snapshot.archived ? UMI_KANBAN_RESTORE_CARD : UMI_KANBAN_ARCHIVE_CARD;
        g_strlcpy(edit.card.id, panel->archive_snapshot.card.id, sizeof edit.card.id);
        status = umi_kanban_board_apply(panel->board, panel->archive_snapshot.board_revision,
            "local-owner", &edit, 1U, &problem);
    }
    if (status == UMI_STATUS_OK) {
        panel->archive_ready = false;
        /* No old card draft gains permission from this independent operation.
         * Its revision remains stale until that form explicitly loads again. */
        (void)UmiKanbanGtk4Refresh(root);
        if (ArchiveLive(panel)) gtk_label_set_text(GTK_LABEL(panel->archive_status),
            "Card archive state changed. Save through the host. Reload cards before another change; no card was deleted.");
    } else if (ArchiveLive(panel)) {
        char message[320];
        g_snprintf(message, sizeof message, "Archive change refused: %s. No card changed. Check active dependents and list limits, then reload and review.", umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->archive_status), message);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreateArchiveForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); panel->archive_form = box;
    gtk_widget_set_name(box, "kanban-card-archive");
    Bind(box, "unmap", G_CALLBACK(ArchiveInvalidate), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("Load active and archived cards, choose one and review the exact action. Apply changes only the archive state. Card data and board capacity remain allocated; archiving is not deletion or permission to publish private details."));
    GtkStringList *empty = gtk_string_list_new(NULL);
    panel->archive_card = umi_ui_gtk4_drop_down_new_take_string_list(empty);
    gtk_widget_set_name(panel->archive_card, "kanban-archive-card");
    gtk_box_append(GTK_BOX(box), panel->archive_card); g_object_ref(panel->archive_card);
    Bind(panel->archive_card, "notify::selected", G_CALLBACK(ArchiveSelection), panel, NULL, NULL, 0U);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load cards for archiving", G_CALLBACK(LoadArchiveCards));
    Button(panel, actions, "Review archive change", G_CALLBACK(ReviewArchiveCard));
    Button(panel, actions, "Apply reviewed archive change", G_CALLBACK(ApplyArchiveCard));
    panel->archive_status = Label("No archive action reviewed.");
    gtk_widget_set_name(panel->archive_status, "kanban-archive-status");
    gtk_box_append(GTK_BOX(box), panel->archive_status); g_object_ref(panel->archive_status);
    return box;
}
