/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_card_dependencies.inc
 *
 * PURPOSE:
 *   Add and remove explicit card prerequisites without relying on mutable list positions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


typedef char KanbanDependencyId[UMI_KANBAN_ID_SIZE];
static bool DependenciesLive(Panel *panel)
{
    return panel->board && !panel->busy && !panel->dependencies_interrupted &&
        gtk_widget_get_mapped(panel->root) && gtk_widget_get_mapped(panel->dependencies_form);
}
static void DependenciesUnmap(GtkWidget *widget, gpointer data)
{
    (void)widget; Binding *binding = data;
    GtkWidget *root = g_weak_ref_get(&binding->root);
    if (!root) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel) { panel->dependencies_interrupted = true; panel->dependencies_ready = false; }
    g_object_unref(root);
}
static bool HasDependency(const UmiKanbanCardSnapshot *card, const char *id)
{
    for (size_t i = 0U; i < card->card.dependency_count; ++i)
        if (!strcmp(card->card.dependencies[i], id)) return true;
    return false;
}
static void AppendDependencyLabel(GtkStringList *list, const UmiKanbanCardSnapshot *card)
{
    char text[384];
    g_snprintf(text, sizeof text, "%s [%s]",
        DetailsText(card->card.title, sizeof card->card.title) ? card->card.title : "Unsupported title encoding",
        card->card.id);
    gtk_string_list_append(list, text);
}
static void LoadDependencies(GtkWidget *button, gpointer data)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!gtk_widget_get_mapped(button) || !gtk_widget_is_ancestor(button, panel->dependencies_form)) goto done;
    panel->applying = true; panel->dependencies_interrupted = false;
    uint64_t revision = panel->selection_revision;
    UmiKanbanCardSnapshot selected;
    UmiStatus status = revision == umi_kanban_board_revision(panel->board)
        ? umi_kanban_board_find(panel->board, panel->selected, &selected) : UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK || selected.archived) {
        if (DependenciesLive(panel)) gtk_label_set_text(GTK_LABEL(panel->dependencies_status),
            "Select a current active card before loading its prerequisites.");
        goto complete;
    }
    /* Build the complete identity-to-label mapping before publishing either
     * dropdown. Small pages avoid a full-size card array on the GTK stack. */
    char (*ids)[UMI_KANBAN_ID_SIZE] = g_new0(KanbanDependencyId, UMI_KANBAN_MAX_CARDS);
    GtkStringList *available = gtk_string_list_new(NULL), *required = gtk_string_list_new(NULL);
    gtk_string_list_append(available, "Choose a prerequisite to add");
    gtk_string_list_append(required, "Choose an existing prerequisite to remove");
    UmiKanbanCardSnapshot *rows = g_new0(UmiKanbanCardSnapshot, 32U);
    size_t offset = 0U, count = 0U, total = 0U, choices = 0U;
    do {
        status = umi_kanban_board_page(panel->board, revision, NULL, offset, rows, 32U, &count, &total);
        if (status != UMI_STATUS_OK) break;
        for (size_t i = 0U; i < count; ++i) {
            if (!strcmp(rows[i].card.id, selected.card.id) || HasDependency(&selected, rows[i].card.id)) continue;
            memcpy(ids[choices++], rows[i].card.id, strlen(rows[i].card.id) + 1U);
            AppendDependencyLabel(available, &rows[i]);
        }
        offset += count;
    } while (offset < total && count != 0U);
    g_free(rows);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < selected.card.dependency_count; ++i) {
        UmiKanbanCardSnapshot dependency;
        status = umi_kanban_board_find(panel->board, selected.card.dependencies[i], &dependency);
        if (status == UMI_STATUS_OK) AppendDependencyLabel(required, &dependency);
    }
    if (status != UMI_STATUS_OK) {
        if (DependenciesLive(panel)) gtk_label_set_text(GTK_LABEL(panel->dependencies_status),
            "Prerequisites could not be captured. The previous choices are retained.");
        goto release;
    }
    panel->dependencies_ready = false;
    g_free(panel->dependency_choices); panel->dependency_choices = ids; ids = NULL;
    panel->dependency_choice_count = choices;
    panel->dependency_existing_count = selected.card.dependency_count;
    memcpy(panel->dependency_existing, selected.card.dependencies, sizeof panel->dependency_existing);
    memcpy(panel->dependency_card, selected.card.id, strlen(selected.card.id) + 1U);
    panel->dependency_revision = revision;
    gtk_drop_down_set_model(GTK_DROP_DOWN(panel->dependency_add), G_LIST_MODEL(available));
    if (!DependenciesLive(panel)) goto release;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->dependency_add), 0U);
    if (!DependenciesLive(panel)) goto release;
    gtk_drop_down_set_model(GTK_DROP_DOWN(panel->dependency_remove), G_LIST_MODEL(required));
    if (!DependenciesLive(panel)) goto release;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->dependency_remove), 0U);
    if (!DependenciesLive(panel)) goto release;
    char message[192];
    g_snprintf(message, sizeof message, "Prerequisites for %s at revision %" G_GUINT64_FORMAT
        ". Choose a row explicitly; the first choice performs no edit.", selected.card.id, (guint64)revision);
    gtk_label_set_text(GTK_LABEL(panel->dependencies_status), message);
    panel->dependencies_ready = DependenciesLive(panel);
release:
    g_free(ids); g_object_unref(available); g_object_unref(required);
complete:
    panel->applying = false;
done:
    g_object_unref(root);
}
static void EditDependency(GtkWidget *button, gpointer data, bool add)
{
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (!panel) return;
    if (!DependenciesLive(panel) || !panel->dependencies_ready || !gtk_widget_get_mapped(button) ||
        !gtk_widget_is_ancestor(button, panel->dependencies_form)) goto done;
    panel->applying = true;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(add ? panel->dependency_add : panel->dependency_remove));
    size_t count = add ? panel->dependency_choice_count : panel->dependency_existing_count;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (index != GTK_INVALID_LIST_POSITION && index > 0U && (size_t)index <= count) {
        const char *required = add ? panel->dependency_choices[index - 1U] : panel->dependency_existing[index - 1U];
        if (strcmp(panel->selected, panel->dependency_card) || panel->selection_revision != panel->dependency_revision)
            status = UMI_STATUS_INVALID_STATE;
        else status = add ? UmiKanbanCardDependencyAdd(panel->board, panel->dependency_revision,
            panel->dependency_card, required, "local-owner") :
            UmiKanbanCardDependencyRemove(panel->board, panel->dependency_revision,
            panel->dependency_card, required, "local-owner");
    }
    if (status == UMI_STATUS_OK) {
        panel->dependencies_ready = false; panel->selection_revision = umi_kanban_board_revision(panel->board);
        (void)UmiKanbanGtk4Refresh(root);
        if (DependenciesLive(panel)) gtk_label_set_text(GTK_LABEL(panel->dependencies_status),
            "Prerequisites changed. Save the board through its host. Load prerequisites again before another edit.");
    } else if (DependenciesLive(panel)) {
        char message[224]; g_snprintf(message, sizeof message,
            "Prerequisite edit refused: %s. Select an explicit choice from a current load. Cycles and unfinished prerequisites on Done cards are not accepted.",
            umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->dependencies_status), message);
    }
    panel->applying = false;
done:
    g_object_unref(root);
}
static void AddDependency(GtkWidget *button, gpointer data) { EditDependency(button, data, true); }
static void RemoveDependency(GtkWidget *button, gpointer data) { EditDependency(button, data, false); }
static GtkWidget *CreateDependenciesForm(Panel *panel)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    panel->dependencies_form = box; gtk_widget_set_name(box, "kanban-card-dependencies");
    Bind(box, "unmap", G_CALLBACK(DependenciesUnmap), panel, NULL, NULL, 0U);
    gtk_box_append(GTK_BOX(box), Label("A prerequisite must be Done before dependent work can enter stages that require completed dependencies. Load the selected card, then choose a prerequisite explicitly. Adding a dependency does not move either card."));
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Load selected prerequisites", G_CALLBACK(LoadDependencies));
    const char *add[] = {"Choose a prerequisite to add", NULL}, *remove[] = {"Choose an existing prerequisite to remove", NULL};
    panel->dependency_add = umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(add));
    panel->dependency_remove = umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(remove));
    gtk_widget_set_name(panel->dependency_add, "kanban-dependency-add");
    gtk_widget_set_name(panel->dependency_remove, "kanban-dependency-remove");
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(panel->dependency_add), TRUE);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(panel->dependency_remove), TRUE);
    gtk_box_append(GTK_BOX(box), panel->dependency_add); g_object_ref(panel->dependency_add);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Add prerequisite", G_CALLBACK(AddDependency));
    gtk_box_append(GTK_BOX(box), panel->dependency_remove); g_object_ref(panel->dependency_remove);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(box), actions);
    Button(panel, actions, "Remove prerequisite", G_CALLBACK(RemoveDependency));
    panel->dependencies_status = Label("No prerequisites loaded.");
    gtk_widget_set_name(panel->dependencies_status, "kanban-dependency-status");
    gtk_box_append(GTK_BOX(box), panel->dependencies_status); g_object_ref(panel->dependencies_status);
    return box;
}
