/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_column_policy.inc
 *
 * PURPOSE:
 *   Review list occupancy and change its work limit at an observed board revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Lists are fixed for a board lifetime. The picker index can identify a list,
 * but only an explicit Load captures authority to edit its current policy. */
static bool PolicyLive(Panel *panel)
{
    return panel->board && !panel->busy && !panel->policy_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->policy_form);
}
static void PolicyInvalidate(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (!root) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel) { panel->policy_ready = false; panel->policy_interrupted = true; }
    g_object_unref(root);
}
static void PolicySelection(GObject *object, GParamSpec *property, gpointer data)
{ (void)property; PolicyInvalidate(GTK_WIDGET(object), data); }
static void LoadPolicy(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->policy_form)) goto done;
    panel->policy_interrupted = false; panel->applying = true; panel->policy_ready = false;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->policy_column));
    UmiKanbanColumnPolicySnapshot snapshot;
    UmiStatus status = selected < panel->config.column_count
        ? UmiKanbanColumnPolicyRead(panel->board, umi_kanban_board_revision(panel->board),
            panel->config.columns[selected].id, &snapshot) : UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) {
        if (PolicyLive(panel)) gtk_label_set_text(GTK_LABEL(panel->policy_status), "Choose a current list before loading its limit.");
        goto complete;
    }
    panel->policy_snapshot = snapshot;
    char limit[32], message[320];
    g_snprintf(limit, sizeof limit, "%zu", snapshot.column.wip_limit);
    gtk_editable_set_text(GTK_EDITABLE(panel->policy_limit), limit);
    if (!PolicyLive(panel)) goto complete;
    g_snprintf(message, sizeof message, "List %s: %zu active cards; board capacity %zu; reviewed revision %" G_GUINT64_FORMAT
        ". Zero removes the limit. Archived cards do not count.",
        snapshot.column.id, snapshot.active_cards, snapshot.board_capacity, (guint64)snapshot.board_revision);
    gtk_label_set_text(GTK_LABEL(panel->policy_status), message);
    panel->policy_ready = PolicyLive(panel) &&
        gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->policy_column)) == selected;
complete:
    panel->applying = false;
done:
    g_object_unref(root);
}
static bool PolicyLimit(const char *text, size_t maximum, size_t *out)
{
    if (!text || !text[0]) return false;
    size_t result = 0U;
    for (size_t i = 0U; text[i]; ++i) {
        if (text[i] < '0' || text[i] > '9') return false;
        size_t digit = (size_t)(text[i] - '0');
        if (digit > maximum || result > (maximum - digit) / 10U) return false;
        result = result * 10U + digit;
    }
    *out = result; return true;
}
static void ApplyPolicy(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!PolicyLive(panel) || !panel->policy_ready || !gtk_widget_get_mapped(button) ||
        !gtk_widget_is_ancestor(button, panel->policy_form)) goto done;
    panel->applying = true;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->policy_column));
    size_t limit = 0U;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (PolicyLimit(gtk_editable_get_text(GTK_EDITABLE(panel->policy_limit)),
        panel->policy_snapshot.board_capacity, &limit)) {
        if (selected >= panel->config.column_count ||
            strcmp(panel->config.columns[selected].id, panel->policy_snapshot.column.id))
            status = UMI_STATUS_INVALID_STATE;
        else status = UmiKanbanColumnPolicyApply(panel->board, panel->policy_snapshot.board_revision,
            panel->policy_snapshot.column.id, "local-owner", limit);
    }
    if (status == UMI_STATUS_OK) {
        panel->policy_ready = false;
        (void)UmiKanbanGtk4Refresh(root);
        if (PolicyLive(panel)) gtk_label_set_text(GTK_LABEL(panel->policy_status),
            "List limit changed. Save the board through its host. Load the list again to review another edit.");
    } else if (PolicyLive(panel)) {
        char message[256];
        g_snprintf(message, sizeof message, "Limit refused: %s. Use a whole number within board capacity; a nonzero limit cannot be below active occupancy. Reload after board changes.", umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->policy_status), message);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreatePolicyForm(Panel *panel, const UmiKanbanConfig *config)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); panel->policy_form = box;
    gtk_widget_set_name(box, "kanban-column-policy");
    Bind(box, "unmap", G_CALLBACK(PolicyInvalidate), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("Choose a list and load its current active-card count and work limit. Limits apply to all active cards in that list; cards are never moved or removed automatically."));
    GtkStringList *names = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < config->column_count; ++i)
        gtk_string_list_append(names, g_utf8_validate(config->columns[i].title, -1, NULL) ? config->columns[i].title : config->columns[i].id);
    panel->policy_column = umi_ui_gtk4_drop_down_new_take_string_list(names);
    gtk_widget_set_name(panel->policy_column, "kanban-policy-column");
    gtk_box_append(GTK_BOX(box), panel->policy_column); g_object_ref(panel->policy_column);
    Bind(panel->policy_column, "notify::selected", G_CALLBACK(PolicySelection), panel, NULL, NULL, 0U);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load list limit", G_CALLBACK(LoadPolicy));
    Button(panel, actions, "Apply list limit", G_CALLBACK(ApplyPolicy));
    panel->policy_limit = Entry(box, "Maximum active cards (0 means unlimited)", 10);
    gtk_widget_set_name(panel->policy_limit, "kanban-policy-limit"); g_object_ref(panel->policy_limit);
    panel->policy_status = Label("No list limit loaded.");
    gtk_widget_set_name(panel->policy_status, "kanban-policy-status");
    gtk_box_append(GTK_BOX(box), panel->policy_status); g_object_ref(panel->policy_status);
    return box;
}
