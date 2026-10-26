/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_view_filter.inc
 *
 * PURPOSE:
 *   Filter native board projections through existing C23 planning queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Filters change only projection, never card records or publication approvals. */
static UmiStatus FilterDeveloper(Panel *panel, const char *text, char out[UMI_KANBAN_ID_SIZE])
{
    if (!text[0]) { out[0] = '\0'; return UMI_STATUS_OK; }
    uint64_t revision = umi_kanban_board_revision(panel->board);
    UmiKanbanDeveloper person;
    if (umi_kanban_board_developer_find(panel->board, revision, text, &person) == UMI_STATUS_OK) {
        g_strlcpy(out, person.id, UMI_KANBAN_ID_SIZE); return UMI_STATUS_OK;
    }
    char *wanted = g_utf8_casefold(text, -1); size_t matches = 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < umi_kanban_board_developer_count(panel->board); ++i) {
        status = umi_kanban_board_developer_at(panel->board, revision, i, &person);
        if (status != UMI_STATUS_OK) break;
        if (!g_utf8_validate(person.name, -1, NULL)) continue;
        char *name = g_utf8_casefold(person.name, -1);
        if (!strcmp(wanted, name)) { ++matches; g_strlcpy(out, person.id, UMI_KANBAN_ID_SIZE); }
        g_free(name);
    }
    g_free(wanted);
    if (status != UMI_STATUS_OK) return status;
    return matches == 1U ? UMI_STATUS_OK : matches == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_ALREADY_EXISTS;
}
static bool FilterLive(Panel *panel)
{
    return panel->board && !panel->busy && gtk_widget_get_mapped(panel->root) &&
        gtk_widget_get_mapped(panel->filter_form);
}
static UmiStatus FilterRead(Panel *panel, UmiKanbanFilter *out)
{
    UmiKanbanFilter filter = {0}; char developer[128];
    if (!CopyEntry(panel->filter_text, filter.text, sizeof filter.text, true) ||
        !CopyEntry(panel->filter_label, filter.label, sizeof filter.label, true) ||
        !CopyEntry(panel->filter_developer, developer, sizeof developer, true)) return UMI_STATUS_INVALID_ARGUMENT;
    guint priority = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->filter_priority));
    if (priority > UMI_KANBAN_URGENT) return UMI_STATUS_INVALID_ARGUMENT;
    filter.minimum_priority = (UmiKanbanPriority)priority;
    filter.blocked_only = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->filter_blocked)) ? 1 : 0;
    filter.overdue_only = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->filter_overdue)) ? 1 : 0;
    if (filter.overdue_only && !DeadlineParse(gtk_editable_get_text(GTK_EDITABLE(panel->filter_time)),
        &filter.now_unix_seconds)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = FilterDeveloper(panel, developer, filter.assignee);
    if (status == UMI_STATUS_OK) *out = filter;
    return status;
}
static UmiStatus PublishFilter(Panel *panel, GtkWidget *root, const UmiKanbanFilter *filter)
{
    UmiKanbanFilter previous = panel->filter; panel->filter = *filter;
    AbandonCardSelection(panel);
    UmiStatus status = UmiKanbanGtk4Refresh(root);
    if (status != UMI_STATUS_OK) panel->filter = previous;
    if (FilterLive(panel)) gtk_label_set_text(GTK_LABEL(panel->filter_status), status == UMI_STATUS_OK
        ? "View updated without editing the board. Select a visible card again before editing. List limits still count all active cards."
        : "The view could not be refreshed. The previous filter is retained.");
    return status;
}
static void ApplyFilter(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (!panel) return;
    if (!FilterLive(panel) || !gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->filter_form)) goto done;
    panel->applying = true; UmiKanbanFilter filter;
    UmiStatus status = FilterRead(panel, &filter);
    if (status == UMI_STATUS_OK) (void)PublishFilter(panel, root, &filter);
    else if (FilterLive(panel)) {
        const char *reason = status == UMI_STATUS_ALREADY_EXISTS
            ? "Several developers have this name. Enter the exact registered ID."
            : status == UMI_STATUS_NOT_FOUND ? "No registered developer matches. Enter an existing name or ID, or leave it blank."
            : "Use bounded UTF-8 fields, a priority and an exact UTC cutoff when overdue filtering is enabled.";
        gtk_label_set_text(GTK_LABEL(panel->filter_status), reason);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static void ShowAllCards(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (!panel) return;
    if (!FilterLive(panel) || !gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->filter_form)) goto done;
    panel->applying = true; UmiKanbanFilter all = {0};
    UmiStatus status = PublishFilter(panel, root, &all);
    if (status == UMI_STATUS_OK && FilterLive(panel)) gtk_label_set_text(GTK_LABEL(panel->filter_status),
        "All active cards are displayed. Filter fields remain as a draft; choose Apply view filter to use them again.");
    panel->applying = false;
done:
    g_object_unref(root);
}
static GtkWidget *CreateFilterForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); panel->filter_form = box;
    gtk_widget_set_name(box, "kanban-view-filter");
    gtk_box_append(GTK_BOX(box), Label("Filter active cards by private text, label, developer, priority or planning state. Fields combine with AND. Filtering does not change assignments, limits or public website selections."));
    panel->filter_text = Entry(box, "Title or description contains (case-sensitive)", 255);
    gtk_widget_set_name(panel->filter_text, "kanban-filter-text"); g_object_ref(panel->filter_text);
    panel->filter_label = Entry(box, "Exact private label (case-sensitive)", 63);
    gtk_widget_set_name(panel->filter_label, "kanban-filter-label"); g_object_ref(panel->filter_label);
    panel->filter_developer = Entry(box, "Registered developer name or ID (blank means anyone)", 127);
    gtk_widget_set_name(panel->filter_developer, "kanban-filter-developer"); g_object_ref(panel->filter_developer);
    const char *priorities[] = {"Any priority", "Normal or higher", "High or higher", "Urgent only", NULL};
    panel->filter_priority = gtk_drop_down_new_from_strings(priorities);
    gtk_widget_set_name(panel->filter_priority, "kanban-filter-priority"); g_object_ref(panel->filter_priority);
    gtk_box_append(GTK_BOX(box), panel->filter_priority);
    panel->filter_blocked = gtk_check_button_new_with_label("Blocked cards only");
    gtk_widget_set_name(panel->filter_blocked, "kanban-filter-blocked"); g_object_ref(panel->filter_blocked);
    gtk_box_append(GTK_BOX(box), panel->filter_blocked);
    panel->filter_overdue = gtk_check_button_new_with_label("Overdue before the UTC cutoff");
    gtk_widget_set_name(panel->filter_overdue, "kanban-filter-overdue"); g_object_ref(panel->filter_overdue);
    gtk_box_append(GTK_BOX(box), panel->filter_overdue);
    panel->filter_time = Entry(box, "UTC cutoff (YYYY-MM-DDTHH:MM:SSZ)", 64);
    gtk_widget_set_name(panel->filter_time, "kanban-filter-time"); g_object_ref(panel->filter_time);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Apply view filter", G_CALLBACK(ApplyFilter));
    Button(panel, actions, "Show all active cards", G_CALLBACK(ShowAllCards));
    panel->filter_status = Label("All active cards are displayed."); g_object_ref(panel->filter_status);
    gtk_widget_set_name(panel->filter_status, "kanban-filter-status"); gtk_box_append(GTK_BOX(box), panel->filter_status);
    return box;
}
