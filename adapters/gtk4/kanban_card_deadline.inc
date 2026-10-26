/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_card_deadline.inc
 *
 * PURPOSE:
 *   Edit explicit UTC card deadlines without reading the host timezone.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Native date display is deliberately narrower than the portable uint64_t
 * archive. GLib handles calendar validation; exact roundtrip comparison rejects
 * offsets, fractional seconds, normalized invalid dates and alternate spelling. */
#define KANBAN_NATIVE_LAST_SECOND UINT64_C(253402300799)
static char *DeadlineText(uint64_t seconds)
{
    if (seconds == 0U || seconds > KANBAN_NATIVE_LAST_SECOND) return NULL;
    GDateTime *date = g_date_time_new_from_unix_utc((gint64)seconds);
    if (!date) return NULL;
    char *text = g_date_time_format(date, "%Y-%m-%dT%H:%M:%SZ");
    g_date_time_unref(date); return text;
}
static bool DeadlineParse(const char *text, uint64_t *out)
{
    if (strlen(text) != 20U || text[19] != 'Z') return false;
    GDateTime *date = g_date_time_new_from_iso8601(text, NULL);
    if (!date) return false;
    gint64 seconds = g_date_time_to_unix(date); g_date_time_unref(date);
    if (seconds <= 0 || (uint64_t)seconds > KANBAN_NATIVE_LAST_SECOND) return false;
    char *canonical = DeadlineText((uint64_t)seconds);
    bool valid = canonical && !strcmp(canonical, text); g_free(canonical);
    if (valid) *out = (uint64_t)seconds;
    return valid;
}
static bool DeadlineLive(Panel *panel)
{
    return panel->board && !panel->busy && !panel->deadline_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->deadline_form);
}
static void DeadlineUnmap(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (!root) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel) { panel->deadline_ready = false; panel->deadline_interrupted = true; }
    g_object_unref(root);
}
static void LoadDeadline(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->deadline_form)) goto done;
    panel->deadline_interrupted = false; panel->applying = true;
    UmiKanbanCardDeadlineSnapshot snapshot;
    UmiStatus status = UmiKanbanCardDeadlineRead(panel->board, panel->selection_revision, panel->selected, &snapshot);
    if (status != UMI_STATUS_OK) {
        if (DeadlineLive(panel)) gtk_label_set_text(GTK_LABEL(panel->deadline_status),
            "Select a current active card before loading its deadline. The prior draft is retained.");
        goto complete;
    }
    panel->deadline_ready = false; panel->deadline_snapshot = snapshot;
    char *date = DeadlineText(snapshot.due_unix_seconds);
    gtk_editable_set_text(GTK_EDITABLE(panel->deadline_value), date ? date : ""); g_free(date);
    if (!DeadlineLive(panel)) goto complete;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->deadline_enabled), snapshot.due_unix_seconds != 0U);
    if (!DeadlineLive(panel)) goto complete;
    char message[320];
    if (snapshot.due_unix_seconds > KANBAN_NATIVE_LAST_SECOND)
        g_snprintf(message, sizeof message, "The current value (%" G_GUINT64_FORMAT
            " UTC seconds) exceeds the native date range and is preserved. Enter a supported UTC date, or explicitly uncheck Use deadline to clear it.",
            (guint64)snapshot.due_unix_seconds);
    else g_snprintf(message, sizeof message, "Deadline for %s at revision %" G_GUINT64_FORMAT
        ". Enter YYYY-MM-DDTHH:MM:SSZ in UTC, or uncheck Use deadline to clear.", snapshot.card_id, (guint64)snapshot.board_revision);
    gtk_label_set_text(GTK_LABEL(panel->deadline_status), message);
    panel->deadline_ready = DeadlineLive(panel);
complete:
    panel->applying = false;
done:
    g_object_unref(root);
}
static void ApplyDeadline(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!DeadlineLive(panel) || !panel->deadline_ready || !gtk_widget_get_mapped(button) ||
        !gtk_widget_is_ancestor(button, panel->deadline_form)) goto done;
    panel->applying = true;
    uint64_t seconds = 0U;
    bool enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->deadline_enabled));
    bool valid = !enabled || DeadlineParse(gtk_editable_get_text(GTK_EDITABLE(panel->deadline_value)), &seconds);
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (valid) {
        if (strcmp(panel->selected, panel->deadline_snapshot.card_id) ||
            panel->selection_revision != panel->deadline_snapshot.board_revision)
            status = UMI_STATUS_INVALID_STATE;
        else status = UmiKanbanCardDeadlineApply(panel->board, panel->deadline_snapshot.board_revision,
            panel->deadline_snapshot.card_id, "local-owner", seconds);
    }
    if (status == UMI_STATUS_OK) {
        panel->deadline_ready = false;
        panel->selection_revision = umi_kanban_board_revision(panel->board);
        (void)UmiKanbanGtk4Refresh(root);
        if (DeadlineLive(panel)) gtk_label_set_text(GTK_LABEL(panel->deadline_status),
            "Deadline changed. Save the board through its host; load again before another edit.");
    } else if (DeadlineLive(panel)) {
        char message[320];
        g_snprintf(message, sizeof message, "Deadline refused: %s. Use an exact UTC date from 1970-01-01T00:00:01Z through 9999-12-31T23:59:59Z. No offsets or fractional seconds. Reload after board changes.", umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->deadline_status), message);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreateDeadlineForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); panel->deadline_form = box;
    gtk_widget_set_name(box, "kanban-card-deadline");
    Bind(box, "unmap", G_CALLBACK(DeadlineUnmap), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("Load the selected card before editing. Deadlines use explicit UTC, not the computer timezone. A deadline is planning metadata; it does not schedule jobs or mark work complete."));
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load selected deadline", G_CALLBACK(LoadDeadline));
    Button(panel, actions, "Apply deadline", G_CALLBACK(ApplyDeadline));
    panel->deadline_enabled = gtk_check_button_new_with_label("Use deadline");
    gtk_widget_set_name(panel->deadline_enabled, "kanban-deadline-enabled");
    gtk_box_append(GTK_BOX(box), panel->deadline_enabled); g_object_ref(panel->deadline_enabled);
    /* Leave room for malformed pasted input so exact parsing can refuse it;
     * truncating at twenty characters could turn a valid prefix into an edit. */
    panel->deadline_value = Entry(box, "UTC deadline (YYYY-MM-DDTHH:MM:SSZ)", 64);
    gtk_widget_set_name(panel->deadline_value, "kanban-deadline-value"); g_object_ref(panel->deadline_value);
    panel->deadline_status = Label("No card deadline loaded.");
    gtk_widget_set_name(panel->deadline_status, "kanban-deadline-status");
    gtk_box_append(GTK_BOX(box), panel->deadline_status); g_object_ref(panel->deadline_status);
    return box;
}
