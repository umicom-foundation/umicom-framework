/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban.c
 *
 * PURPOSE:
 *   Share native card editing, named assignment, drag movement and deliberate website export.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/kanban/developers.h"
#include "umicom/kanban/publication.h"
#include "umicom/ui/gtk4/kanban_publication_store.h"
#include "umicom/ui/gtk4/kanban_completion.h"
#include "umicom/ui/gtk4/drop_down.h"
#include "umicom/kanban/view.h"
#include "umicom/kanban/card_details.h"
#include "umicom/kanban/column_policy.h"
#include <string.h>
#include "focus_handoff_private.h"

#define PANEL_KEY "umicom-kanban-panel"
typedef struct Panel {
    UmiKanbanBoard *board;
    GtkWidget *root, *lanes, *status, *title, *developer, *suggestions, *destination;
    GtkWidget *public_title, *public_summary, *public_developer, *site_title, *export_control;
    GtkTextBuffer *html;
    GtkWidget *public_plan_store;
    GtkWidget *completion_review;
    GtkWidget *details_form, *details_fields[2], *details_priority, *details_status;
    GtkTextBuffer *details_description;
    UmiKanbanCardDetailSnapshot details_snapshot;
    bool details_ready, details_interrupted;
    GtkWidget *checklist_form, *checklist_status;
    GtkWidget *checklist_text[UMI_KANBAN_MAX_CHECKS], *checklist_complete[UMI_KANBAN_MAX_CHECKS];
    UmiKanbanCardChecklistSnapshot checklist_snapshot;
    bool checklist_ready, checklist_interrupted;
    GtkWidget *dependencies_form, *dependency_add, *dependency_remove, *dependencies_status;
    char (*dependency_choices)[UMI_KANBAN_ID_SIZE];
    char dependency_existing[UMI_KANBAN_MAX_DEPENDENCIES][UMI_KANBAN_ID_SIZE];
    char dependency_card[UMI_KANBAN_ID_SIZE];
    size_t dependency_choice_count, dependency_existing_count;
    uint64_t dependency_revision;
    bool dependencies_ready, dependencies_interrupted;
    GtkWidget *labels_form, *labels_status, *labels_text[UMI_KANBAN_MAX_LABELS];
    UmiKanbanCardLabelsSnapshot labels_snapshot;
    bool labels_ready, labels_interrupted;
    GtkWidget *policy_form, *policy_column, *policy_limit, *policy_status;
    UmiKanbanColumnPolicySnapshot policy_snapshot;
    bool policy_ready, policy_interrupted;
    GtkWidget *deadline_form, *deadline_enabled, *deadline_value, *deadline_status;
    UmiKanbanCardDeadlineSnapshot deadline_snapshot;
    bool deadline_ready, deadline_interrupted;
    GtkWidget *archive_form, *archive_card, *archive_status;
    char (*archive_ids)[UMI_KANBAN_ID_SIZE];
    size_t archive_count;
    uint64_t archive_revision;
    UmiKanbanCardSnapshot archive_snapshot;
    bool archive_ready, archive_interrupted;
    UmiKanbanConfig config;
    UmiKanbanFilter filter;
    GtkWidget *filter_form, *filter_text, *filter_label, *filter_developer, *filter_priority;
    GtkWidget *filter_blocked, *filter_overdue, *filter_time, *filter_status;
    UmiKanbanPublicCard *publication;
    size_t publication_count;
    uint64_t revision, drag_revision, selection_revision;
    char selected[UMI_KANBAN_ID_SIZE], drag_card[UMI_KANBAN_ID_SIZE];
    char *drag_token;
    bool busy, refreshing, applying;
    bool html_current, html_invalidating, html_publishing;
    uint64_t public_generation, html_generation, html_revision;
} Panel;
typedef struct Binding {
    GWeakRef root;
    char id[UMI_KANBAN_ID_SIZE], column[UMI_KANBAN_ID_SIZE];
    uint64_t revision;
} Binding;
static void FreeBinding(gpointer data, GClosure *closure)
{
    (void)closure; Binding *binding = data;
    g_weak_ref_clear(&binding->root); g_free(binding);
}
static void Bind(gpointer object, const char *signal, GCallback callback,
    Panel *panel, const char *id, const char *column, uint64_t revision)
{
    Binding *binding = g_new0(Binding, 1);
    g_weak_ref_init(&binding->root, G_OBJECT(panel->root));
    if (id != NULL) g_strlcpy(binding->id, id, sizeof(binding->id));
    if (column != NULL) g_strlcpy(binding->column, column, sizeof(binding->column));
    binding->revision = revision;
    g_signal_connect_data(object, signal, callback, binding, FreeBinding, 0);
}
static Panel *Acquire(Binding *binding, GtkWidget **root)
{
    *root = g_weak_ref_get(&binding->root);
    if (*root == NULL) return NULL;
    Panel *panel = g_object_get_data(G_OBJECT(*root), PANEL_KEY);
    if (panel == NULL || panel->board == NULL || panel->busy || panel->refreshing || panel->applying) {
        g_object_unref(*root); *root = NULL; return NULL;
    }
    return panel;
}
static void Message(Panel *panel, const char *text) { gtk_label_set_text(GTK_LABEL(panel->status), text); }
static void FreePanel(gpointer data)
{
    Panel *panel = data;
    /* Retain the export control independently of GTK child disposal order. */
    if (panel->export_control != NULL) {
        UmiTextExportGtk4Retire(panel->export_control);
        g_object_unref(panel->export_control);
    }
    if(panel->public_plan_store!=NULL){
        UmiKanbanPublicPlanGtk4Retire(panel->public_plan_store);
        g_object_unref(panel->public_plan_store);
    }
    if (panel->completion_review != NULL) {
        UmiKanbanCompletionGtk4Retire(panel->completion_review);
        g_object_unref(panel->completion_review);
    }
    g_clear_object(&panel->details_fields[0]); g_clear_object(&panel->details_fields[1]);
    g_clear_object(&panel->details_priority); g_clear_object(&panel->details_status);
    g_clear_object(&panel->details_description);
    for (size_t i = 0U; i < UMI_KANBAN_MAX_CHECKS; ++i) {
        g_clear_object(&panel->checklist_text[i]); g_clear_object(&panel->checklist_complete[i]);
    }
    g_clear_object(&panel->checklist_status);
    for (size_t i = 0U; i < UMI_KANBAN_MAX_LABELS; ++i) g_clear_object(&panel->labels_text[i]);
    g_clear_object(&panel->labels_status);
    g_clear_object(&panel->deadline_enabled); g_clear_object(&panel->deadline_value); g_clear_object(&panel->deadline_status);
    g_clear_object(&panel->filter_text); g_clear_object(&panel->filter_label); g_clear_object(&panel->filter_developer);
    g_clear_object(&panel->filter_priority); g_clear_object(&panel->filter_blocked); g_clear_object(&panel->filter_overdue);
    g_clear_object(&panel->filter_time); g_clear_object(&panel->filter_status);
    g_clear_object(&panel->archive_card); g_clear_object(&panel->archive_status); g_free(panel->archive_ids);
    g_clear_object(&panel->policy_column); g_clear_object(&panel->policy_limit); g_clear_object(&panel->policy_status);
    g_clear_object(&panel->dependency_add); g_clear_object(&panel->dependency_remove);
    g_clear_object(&panel->dependencies_status); g_free(panel->dependency_choices);
    g_free(panel->drag_token); g_free(panel->publication); g_free(panel);
}
static GtkWidget *Label(const char *text)
{
    GtkWidget *label = gtk_label_new(g_utf8_validate(text, -1, NULL) ? text : "Unsupported text encoding");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    return label;
}
static GtkWidget *Entry(GtkWidget *box, const char *label, int limit)
{
    gtk_box_append(GTK_BOX(box), Label(label));
    GtkWidget *entry = gtk_entry_new(); gtk_entry_set_max_length(GTK_ENTRY(entry), limit);
    gtk_widget_set_hexpand(entry, TRUE); gtk_box_append(GTK_BOX(box), entry); return entry;
}
static bool CopyEntry(GtkWidget *entry, char *out, size_t capacity, bool optional)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    size_t length = strlen(text);
    if ((!optional && length == 0U) || length >= capacity || !g_utf8_validate(text, -1, NULL)) return false;
    memcpy(out, text, length + 1U); return true;
}
/* Reviewed publication freshness is shared by edits, title changes and export. */
#include "kanban_publication_freshness.inc"

static UmiStatus Apply(Panel *panel, uint64_t revision, const UmiKanbanEdit *edits, size_t count)
{
    /* Completion messages are observable too; serialize the entire edit
     * feedback path, not just the model mutation and row replacement. */
    panel->applying = true;
    UmiKanbanProblem problem;
    UmiStatus status = umi_kanban_board_apply(panel->board, revision, "local-owner", edits, count, &problem);
    if (status == UMI_STATUS_OK) {
        /* Only our own accepted edit can advance the selected form token.
         * An external refresh must not authorize overwriting a newer title
         * with an older editor draft. */
        if (panel->selection_revision == revision)
            panel->selection_revision = umi_kanban_board_revision(panel->board);
        (void)UmiKanbanGtk4Refresh(panel->root);
        Message(panel, "Board changed. Save the board through its host before closing.");
    } else if (problem.kind == UMI_KANBAN_PROBLEM_STALE) {
        Message(panel, "The board changed since this view. Refresh and review the action again.");
    } else {
        char message[192];
        g_snprintf(message, sizeof(message), "Change refused: %s (board rule %d). The previous state is retained.",
            umi_status_text(status), (int)problem.kind); Message(panel, message);
    }
    panel->applying = false;
    return status;
}
/* The former revision-only row callback is retained for review. The
 * replacement checks current native ancestry and guards observable selection
 * updates, preventing detached or nested callbacks from selecting another card. */
#if 0
static void SelectCard(GtkWidget *button, gpointer data)
{
    (void)button; Binding *binding = data; GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel == NULL) return;
    UmiKanbanCardSnapshot row;
    if (binding->revision == umi_kanban_board_revision(panel->board) &&
        umi_kanban_board_find(panel->board, binding->id, &row) == UMI_STATUS_OK) {
        UmiKanbanDeveloper person;
        char owner[128] = "";
        if (row.card.assignee[0] != '\0') {
            if (umi_kanban_board_developer_find(panel->board, binding->revision, row.card.assignee, &person) == UMI_STATUS_OK)
                g_strlcpy(owner, person.name, sizeof(owner));
            else g_strlcpy(owner, row.card.assignee, sizeof(owner));
        }
        /* Returning to a formerly selected card must not revive a draft that
         * was abandoned for another selection, even at the same revision. */
        if (panel->details_ready && strcmp(panel->details_snapshot.card_id, row.card.id) != 0)
            panel->details_ready = false;
        if (panel->checklist_ready && strcmp(panel->checklist_snapshot.card_id, row.card.id) != 0)
            panel->checklist_ready = false;
        if (panel->dependencies_ready && strcmp(panel->dependency_card, row.card.id) != 0)
            panel->dependencies_ready = false;
        if (panel->labels_ready && strcmp(panel->labels_snapshot.card_id, row.card.id) != 0)
            panel->labels_ready = false;
        if (panel->deadline_ready && strcmp(panel->deadline_snapshot.card_id, row.card.id) != 0)
            panel->deadline_ready = false;
        g_strlcpy(panel->selected, row.card.id, sizeof(panel->selected));
        panel->selection_revision = binding->revision;
        gtk_editable_set_text(GTK_EDITABLE(panel->title), g_utf8_validate(row.card.title, -1, NULL) ? row.card.title : "");
        gtk_editable_set_text(GTK_EDITABLE(panel->developer), g_utf8_validate(owner, -1, NULL) ? owner : "");
        Message(panel, "Card selected. Edit its title, assign a developer or choose a destination.");
    } else Message(panel, "Refresh the board before selecting this older card view.");
    g_object_unref(root);
}
#endif

/* A callback may retain a row after Refresh removed it without changing the
 * board revision. Native ancestry, not the revision alone, proves that the
 * callback still belongs to the current presentation. Unparented fixture roots
 * retain their established programmatic interface while detached rows do not. */
static bool CardSourceAttached(Panel *panel, GtkWidget *widget)
{
    return GTK_IS_WIDGET(widget) && gtk_widget_is_ancestor(widget, panel->lanes);
}
static bool SelectedSourceCurrent(Panel *panel, GtkWidget *widget, uint64_t revision)
{
    return panel->board && !panel->busy && CardSourceAttached(panel, widget) &&
        umi_kanban_board_revision(panel->board) == revision;
}
static void AbandonCardSelection(Panel *panel)
{
    panel->selected[0] = '\0'; panel->selection_revision = 0U;
    panel->details_ready = false; panel->checklist_ready = false;
    panel->dependencies_ready = false; panel->labels_ready = false; panel->deadline_ready = false;
}
static void SelectCard(GtkWidget *button, gpointer data)
{
    Binding *binding = data; GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel == NULL) return;
    if (!CardSourceAttached(panel, button)) { g_object_unref(root); return; }
    panel->applying = true;
    UmiKanbanCardSnapshot row;
    if (binding->revision == umi_kanban_board_revision(panel->board) &&
        umi_kanban_board_find(panel->board, binding->id, &row) == UMI_STATUS_OK && !row.archived) {
        UmiKanbanDeveloper person; char owner[128] = "";
        if (row.card.assignee[0] != '\0') {
            if (umi_kanban_board_developer_find(panel->board, binding->revision, row.card.assignee, &person) == UMI_STATUS_OK)
                g_strlcpy(owner, person.name, sizeof owner);
            else g_strlcpy(owner, row.card.assignee, sizeof owner);
        }
        if (panel->details_ready && strcmp(panel->details_snapshot.card_id, row.card.id)) panel->details_ready = false;
        if (panel->checklist_ready && strcmp(panel->checklist_snapshot.card_id, row.card.id)) panel->checklist_ready = false;
        if (panel->dependencies_ready && strcmp(panel->dependency_card, row.card.id)) panel->dependencies_ready = false;
        if (panel->labels_ready && strcmp(panel->labels_snapshot.card_id, row.card.id)) panel->labels_ready = false;
        if (panel->deadline_ready && strcmp(panel->deadline_snapshot.card_id, row.card.id)) panel->deadline_ready = false;
        g_strlcpy(panel->selected, row.card.id, sizeof panel->selected); panel->selection_revision = binding->revision;
        /* Editable notifications can run application code. Guard the whole
         * selection and recheck ownership before publishing its next field. */
        gtk_editable_set_text(GTK_EDITABLE(panel->title), g_utf8_validate(row.card.title, -1, NULL) ? row.card.title : "");
        if (!SelectedSourceCurrent(panel, button, binding->revision)) goto interrupted;
        gtk_editable_set_text(GTK_EDITABLE(panel->developer), g_utf8_validate(owner, -1, NULL) ? owner : "");
        if (!SelectedSourceCurrent(panel, button, binding->revision)) goto interrupted;
        Message(panel, "Card selected. Edit its title, assign a developer or choose a destination.");
        if (!SelectedSourceCurrent(panel, button, binding->revision)) goto interrupted;
    } else Message(panel, "Refresh the board before selecting this older card view.");
    goto complete;
interrupted:
    AbandonCardSelection(panel);
complete:
    panel->applying = false; g_object_unref(root);
}
static void AddCard(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1);
    if (!CopyEntry(panel->title, edit->card.title, sizeof(edit->card.title), false)) {
        Message(panel, "Enter a title that fits within 255 UTF-8 bytes."); goto done;
    }
    edit->kind = UMI_KANBAN_CREATE_CARD; edit->card.priority = UMI_KANBAN_NORMAL;
    char *id = g_uuid_string_random(); g_strlcpy(edit->card.id, id, sizeof(edit->card.id)); g_free(id);
    bool found = false;
    for (size_t i = 0U; i < panel->config.column_count; ++i) if (panel->config.columns[i].stage == UMI_KANBAN_TODO) {
        g_strlcpy(edit->column_id, panel->config.columns[i].id, sizeof(edit->column_id)); found = true; break;
    }
    if (!found) { Message(panel, "This board has no To do column for new cards."); goto done; }
    char selected[UMI_KANBAN_ID_SIZE]; g_strlcpy(selected, edit->card.id, sizeof(selected));
    uint64_t submitted = panel->revision;
    if (Apply(panel, submitted, edit, 1U) == UMI_STATUS_OK) {
        g_strlcpy(panel->selected, selected, sizeof(panel->selected));
        panel->selection_revision = submitted + 1U;
    }
done:
    g_free(edit); g_object_unref(root);
}
static void RenameCard(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    UmiKanbanCardSnapshot row; UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1);
    if (umi_kanban_board_find(panel->board, panel->selected, &row) != UMI_STATUS_OK) Message(panel, "Select a card first.");
    else {
        edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = row.card;
        if (!CopyEntry(panel->title, edit->card.title, sizeof(edit->card.title), false)) Message(panel, "Enter a title within 255 UTF-8 bytes.");
        else (void)Apply(panel, panel->selection_revision, edit, 1U);
    }
    g_free(edit); g_object_unref(root);
}
static void AssignName(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    UmiKanbanEdit *edits = g_new0(UmiKanbanEdit, 2);
    char name[128] = "", developer_id[UMI_KANBAN_ID_SIZE] = "";
    if (panel->selected[0] == '\0') { Message(panel, "Select a card before assigning a developer."); goto done; }
    if (!CopyEntry(panel->developer, name, sizeof(name), true)) { Message(panel, "Use a developer name within 127 UTF-8 bytes."); goto done; }
    g_strstrip(name);
    size_t count = 1U, matches = 0U;
    if (name[0] != '\0') {
        char *folded = g_utf8_casefold(name, -1);
        for (size_t i = 0U; i < umi_kanban_board_developer_count(panel->board); ++i) {
            UmiKanbanDeveloper person;
            if (umi_kanban_board_developer_at(panel->board, panel->revision, i, &person) != UMI_STATUS_OK) continue;
            if (!g_utf8_validate(person.name, -1, NULL)) continue;
            char *candidate = g_utf8_casefold(person.name, -1);
            if (strcmp(candidate, folded) == 0) { ++matches; g_strlcpy(developer_id, person.id, sizeof(developer_id)); }
            g_free(candidate);
        }
        g_free(folded);
        if (matches > 1U) { Message(panel, "Several developers have that name. Rename their directory entries to distinguish them before assigning."); goto done; }
        if (matches == 0U) {
            char *id = g_uuid_string_random(); g_strlcpy(developer_id, id, sizeof(developer_id)); g_free(id);
            edits[0].kind = UMI_KANBAN_PUT_DEVELOPER;
            g_strlcpy(edits[0].card.id, developer_id, sizeof(edits[0].card.id));
            g_strlcpy(edits[0].card.title, name, sizeof(edits[0].card.title)); count = 2U;
        }
    }
    edits[count - 1U].kind = UMI_KANBAN_ASSIGN_CARD;
    g_strlcpy(edits[count - 1U].card.id, panel->selected, sizeof(edits[count - 1U].card.id));
    g_strlcpy(edits[count - 1U].card.assignee, developer_id, sizeof(edits[count - 1U].card.assignee));
    (void)Apply(panel, panel->selection_revision, edits, count);
done:
    g_free(edits); g_object_unref(root);
}
static void Suggest(GtkEditable *entry, gpointer data)
{
    (void)entry; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    const char *text = gtk_editable_get_text(GTK_EDITABLE(panel->developer));
    char *query = g_utf8_casefold(text, -1); GString *names = g_string_new("Matching developers: "); size_t shown = 0U;
    for (size_t i = 0U; i < umi_kanban_board_developer_count(panel->board) && shown < 6U; ++i) {
        UmiKanbanDeveloper person;
        if (umi_kanban_board_developer_at(panel->board, panel->revision, i, &person) != UMI_STATUS_OK || !g_utf8_validate(person.name, -1, NULL)) continue;
        char *name = g_utf8_casefold(person.name, -1);
        if (strstr(name, query) != NULL) { if (shown != 0U) g_string_append(names, ", "); g_string_append(names, person.name); ++shown; }
        g_free(name);
    }
    if (shown == 0U) g_string_append(names, "none; Assign name creates a directory entry for a new name.");
    gtk_label_set_text(GTK_LABEL(panel->suggestions), names->str); g_string_free(names, TRUE); g_free(query); g_object_unref(root);
}
static void MoveSelected(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    guint column = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->destination));
    if (panel->selected[0] == '\0' || column >= panel->config.column_count) Message(panel, "Select a card and destination first.");
    else {
        UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1); edit->kind = UMI_KANBAN_MOVE_CARD;
        g_strlcpy(edit->card.id, panel->selected, sizeof(edit->card.id));
        g_strlcpy(edit->column_id, panel->config.columns[column].id, sizeof(edit->column_id));
        (void)Apply(panel, panel->selection_revision, edit, 1U); g_free(edit);
    }
    g_object_unref(root);
}
/* Previous drag callbacks are retained for review. Current callbacks also
 * require an attached source and target; refresh cancels outstanding drag state
 * even when the board itself did not change. No card deletion occurs here. */
#if 0
static GdkContentProvider *PrepareDrag(GtkDragSource *source, double x, double y, gpointer data)
{
    (void)source; (void)x; (void)y; Binding *binding = data; GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel == NULL) return NULL;
    GdkContentProvider *provider = NULL;
    if (binding->revision == umi_kanban_board_revision(panel->board)) {
        g_strlcpy(panel->drag_card, binding->id, sizeof(panel->drag_card)); panel->drag_revision = binding->revision;
        provider = gdk_content_provider_new_typed(G_TYPE_STRING, panel->drag_token);
    }
    g_object_unref(root); return provider;
}
static void EndDrag(GtkDragSource *source, GdkDrag *drag, gboolean delete_data, gpointer data)
{
    (void)source; (void)drag; (void)delete_data;
    /* The drop transaction already moves the owned row. Never delete it here. */
    GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (panel != NULL) { panel->drag_card[0] = '\0'; g_object_unref(root); }
}
static gboolean Drop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer data)
{
    (void)target; (void)x; (void)y; Binding *binding = data; GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel == NULL) return FALSE;
    gboolean accepted = FALSE;
    if (G_VALUE_HOLDS_STRING(value) && panel->drag_card[0] != '\0' &&
        g_strcmp0(g_value_get_string(value), panel->drag_token) == 0) {
        UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1); edit->kind = UMI_KANBAN_MOVE_CARD;
        g_strlcpy(edit->card.id, panel->drag_card, sizeof(edit->card.id));
        g_strlcpy(edit->column_id, binding->column, sizeof(edit->column_id));
        uint64_t revision = panel->drag_revision; panel->drag_card[0] = '\0';
        accepted = Apply(panel, revision, edit, 1U) == UMI_STATUS_OK; g_free(edit);
    }
    g_object_unref(root); return accepted;
}
#endif

static GdkContentProvider *PrepareDrag(GtkDragSource *source, double x, double y, gpointer data)
{
    (void)x; (void)y; Binding *binding = data; GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel == NULL) return NULL;
    GdkContentProvider *provider = NULL; UmiKanbanCardSnapshot row;
    GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(source));
    if (SelectedSourceCurrent(panel, widget, binding->revision) &&
        umi_kanban_board_find(panel->board, binding->id, &row) == UMI_STATUS_OK && !row.archived) {
        g_strlcpy(panel->drag_card, binding->id, sizeof panel->drag_card); panel->drag_revision = binding->revision;
        provider = gdk_content_provider_new_typed(G_TYPE_STRING, panel->drag_token);
    }
    g_object_unref(root); return provider;
}
static void EndDrag(GtkDragSource *source, GdkDrag *drag, gboolean delete_data, gpointer data)
{
    (void)drag; (void)delete_data; Binding *binding = data;
    GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel != NULL) {
        GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(source));
        /* A late signal from an old row must not cancel a newly prepared drag. */
        if (CardSourceAttached(panel, widget) && binding->revision == panel->drag_revision &&
            !strcmp(binding->id, panel->drag_card)) panel->drag_card[0] = '\0';
        g_object_unref(root);
    }
}
static gboolean Drop(GtkDropTarget *target, const GValue *value, double x, double y, gpointer data)
{
    (void)x; (void)y; Binding *binding = data; GtkWidget *root = NULL; Panel *panel = Acquire(binding, &root);
    if (panel == NULL) return FALSE;
    gboolean accepted = FALSE; GtkWidget *widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(target));
    if (SelectedSourceCurrent(panel, widget, binding->revision) && binding->revision == panel->drag_revision &&
        G_VALUE_HOLDS_STRING(value) && panel->drag_card[0] != '\0' &&
        g_strcmp0(g_value_get_string(value), panel->drag_token) == 0) {
        UmiKanbanEdit *edit = g_new0(UmiKanbanEdit, 1); edit->kind = UMI_KANBAN_MOVE_CARD;
        g_strlcpy(edit->card.id, panel->drag_card, sizeof edit->card.id);
        g_strlcpy(edit->column_id, binding->column, sizeof edit->column_id);
        uint64_t revision = panel->drag_revision; panel->drag_card[0] = '\0';
        accepted = Apply(panel, revision, edit, 1U) == UMI_STATUS_OK; g_free(edit);
    }
    g_object_unref(root); return accepted;
}
static void RefreshClick(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (panel != NULL) { (void)UmiKanbanGtk4Refresh(root); g_object_unref(root); }
}
/* Preserve the former preview flow for review. The replacement invalidates
 * derived HTML and guards native notifications before accepting an export. */
#if 0
static void IncludePublic(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    UmiKanbanPublicCard card = {0}; UmiKanbanCardSnapshot row;
    if (umi_kanban_board_find(panel->board, panel->selected, &row) != UMI_STATUS_OK || row.archived) { Message(panel, "Select an active card to review for publication."); goto done; }
    if (!CopyEntry(panel->public_title, card.title, sizeof(card.title), false) ||
        !CopyEntry(panel->public_summary, card.summary, sizeof(card.summary), true) ||
        !CopyEntry(panel->public_developer, card.developer, sizeof(card.developer), true)) {
        Message(panel, "Enter a public title and bounded public fields. Private card text is not copied automatically."); goto done;
    }
    g_strlcpy(card.card_id, panel->selected, sizeof(card.card_id));
    size_t index = panel->publication_count;
    for (size_t i = 0U; i < panel->publication_count; ++i) if (strcmp(panel->publication[i].card_id, card.card_id) == 0) { index = i; break; }
    if (index == UMI_KANBAN_MAX_CARDS) { Message(panel, "The public selection is full."); goto done; }
    if (index == panel->publication_count) ++panel->publication_count;
    panel->publication[index] = card;
    Message(panel, "Reviewed fields added to the public selection. Generate HTML to inspect the complete snapshot before saving.");
done:
    g_object_unref(root);
}

#endif
static void IncludePublic(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    panel->applying=true;
    UmiKanbanPublicCard card = {0}; UmiKanbanCardSnapshot row;
    if (umi_kanban_board_find(panel->board, panel->selected, &row) != UMI_STATUS_OK || row.archived) { Message(panel, "Select an active card to review for publication."); goto done; }
    if (!CopyEntry(panel->public_title, card.title, sizeof(card.title), false) ||
        !CopyEntry(panel->public_summary, card.summary, sizeof(card.summary), true) ||
        !CopyEntry(panel->public_developer, card.developer, sizeof(card.developer), true)) {
        Message(panel, "Enter a public title and bounded public fields. Private card text is not copied automatically."); goto done;
    }
    g_strlcpy(card.card_id, panel->selected, sizeof(card.card_id));
    size_t index = panel->publication_count;
    for (size_t i = 0U; i < panel->publication_count; ++i) if (strcmp(panel->publication[i].card_id, card.card_id) == 0) { index = i; break; }
    if (index == UMI_KANBAN_MAX_CARDS) { Message(panel, "The public selection is full."); goto done; }
    if (index == panel->publication_count) ++panel->publication_count;
    panel->publication[index] = card;
    InvalidatePublic(panel);
    Message(panel, "Reviewed fields added to the public selection. Generate HTML to inspect the complete snapshot before saving.");
done:
    panel->applying=false;
    g_object_unref(root);
}
/* Preserve the former preview flow for review. The replacement invalidates
 * derived HTML and guards native notifications before accepting an export. */
#if 0
static void ClearPublic(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root);
    if (panel != NULL) { panel->publication_count = 0U; gtk_text_buffer_set_text(panel->html, "", 0); Message(panel, "Public selection cleared."); g_object_unref(root); }
}

#endif
static void ClearPublic(GtkWidget *button,gpointer data)
{
    (void)button;GtkWidget *root=NULL;Panel *panel=Acquire(data,&root);
    if(panel==NULL)return;
    panel->applying=true;panel->publication_count=0U;InvalidatePublic(panel);
    if(panel->board!=NULL)Message(panel,"Public selection cleared. Regenerate the website before exporting.");
    panel->applying=false;g_object_unref(root);
}
/* Preserve the former preview flow for review. The replacement invalidates
 * derived HTML and guards native notifications before accepting an export. */
#if 0
static void GeneratePublic(GtkWidget *button, gpointer data)
{
    (void)button; GtkWidget *root = NULL; Panel *panel = Acquire(data, &root); if (panel == NULL) return;
    const char *title = gtk_editable_get_text(GTK_EDITABLE(panel->site_title)); size_t size = 0U;
    UmiStatus status = umi_kanban_publication_html(panel->board, panel->revision, title, panel->publication,
        panel->publication_count, NULL, 0U, &size);
    if (status != UMI_STATUS_OK) { Message(panel, "Public snapshot refused. Refresh and review the selected cards and public fields."); g_object_unref(root); return; }
    char *html = g_try_malloc(size);
    if (html == NULL) { Message(panel, "Unable to allocate the website snapshot."); g_object_unref(root); return; }
    status = umi_kanban_publication_html(panel->board, panel->revision, title, panel->publication,
        panel->publication_count, html, size, &size);
    if (status == UMI_STATUS_OK) {
        gtk_text_buffer_set_text(panel->html, html, (gint)(size - 1U));
        Message(panel, "Website source prepared from reviewed public fields. Save to a new HTML file; upload only that file to your host.");
    }
    g_free(html); g_object_unref(root);
}

#endif
static void GeneratePublic(GtkWidget *button,gpointer data)
{
    (void)button;GtkWidget *root=NULL;Panel *panel=Acquire(data,&root);
    if(panel==NULL)return;
    panel->applying=true;InvalidatePublic(panel);
    if(panel->board==NULL){panel->applying=false;g_object_unref(root);return;}
    const uint64_t generation=panel->public_generation,revision=panel->revision;
    char *title=g_strdup(gtk_editable_get_text(GTK_EDITABLE(panel->site_title)));
    size_t size=0U;
    UmiStatus status=generation==UINT64_MAX?UMI_STATUS_CAPACITY_EXCEEDED:
        umi_kanban_publication_html(panel->board,revision,title,panel->publication,
            panel->publication_count,NULL,0U,&size);
    char *html=status==UMI_STATUS_OK?g_try_malloc(size):NULL;
    if(status==UMI_STATUS_OK&&html==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=umi_kanban_publication_html(panel->board,revision,title,
        panel->publication,panel->publication_count,html,size,&size);
    if(status==UMI_STATUS_OK){
        GtkTextBuffer *buffer=g_object_ref(panel->html);
        panel->html_publishing=true;
        gtk_text_buffer_set_text(buffer,html,(gint)(size-1U));
        panel->html_publishing=false;
        /* Rendering and GTK publication are different boundaries. A native
         * observer can edit the title or retire the board while text changes. */
        if(panel->board!=NULL&&umi_kanban_board_revision(panel->board)==revision&&
            panel->public_generation==generation){
            panel->html_revision=revision;panel->html_generation=generation;panel->html_current=true;
        }else{InvalidatePublic(panel);status=UMI_STATUS_INVALID_STATE;}
        g_object_unref(buffer);
    }
    if(panel->board!=NULL)Message(panel,status==UMI_STATUS_OK?
        "Current website source prepared from reviewed public fields. Save to a new HTML file; upload only that file to your host.":
        "Public snapshot refused or changed during review. Refresh, check the selected cards and public fields, then generate again.");
    g_free(title);g_free(html);panel->applying=false;g_object_unref(root);
}
/* Shared persistence operates on immutable reviewed fields, never board I/O. */
#include "kanban_publication_plan.inc"
#include "kanban_completion_bridge.inc"

static void Button(Panel *panel, GtkWidget *box, const char *text, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(text); gtk_box_append(GTK_BOX(box), button);
    Bind(button, "clicked", callback, panel, NULL, NULL, 0U);
}
/* Reuse the board revision contract for private detail forms in every host. */
#include "kanban_card_details.inc"
#include "kanban_card_labels.inc"
#include "kanban_card_deadline.inc"
#include "kanban_card_archive.inc"
#include "kanban_view_filter.inc"
#include "kanban_column_policy.inc"
#include "kanban_card_checklist.inc"
#include "kanban_card_dependencies.inc"

UmiStatus UmiKanbanGtk4Refresh(GtkWidget *root)
{
    if (!GTK_IS_WIDGET(root)) return UMI_STATUS_INVALID_ARGUMENT;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel == NULL || panel->board == NULL || panel->busy || panel->refreshing) return UMI_STATUS_UNAVAILABLE;
    g_object_ref(root);
    UmiKanbanConfig config; uint64_t revision = umi_kanban_board_revision(panel->board);
    UmiStatus status = umi_kanban_board_config(panel->board, &config);
    size_t count = 0U, total = 0U;
    UmiKanbanCardSnapshot *rows = g_try_new0(UmiKanbanCardSnapshot, UMI_KANBAN_MAX_CARDS);
    char (*names)[128] = g_try_malloc0(UMI_KANBAN_MAX_CARDS * sizeof(*names));
    if (rows == NULL || names == NULL) { g_free(rows); g_free(names); g_object_unref(root); return UMI_STATUS_OUT_OF_MEMORY; }
    /* The previous unfiltered projection is retained for review. Query rows
     * through the shared filter while capturing unfiltered occupancy separately. */
#if 0
    if (status == UMI_STATUS_OK) status = umi_kanban_board_page(panel->board, revision, NULL, 0U, rows, UMI_KANBAN_MAX_CARDS, &count, &total);
#endif
    UmiKanbanSummary summary;
    if (status == UMI_STATUS_OK) status = umi_kanban_board_summary(panel->board, revision, 0U, &summary);
    if (status == UMI_STATUS_OK) status = umi_kanban_board_page(panel->board, revision, &panel->filter, 0U, rows, UMI_KANBAN_MAX_CARDS, &count, &total);
    /* Snapshot all borrowed model data before GTK notifications can retire a host. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        UmiKanbanDeveloper person;
        if (rows[i].card.assignee[0] != '\0' && umi_kanban_board_developer_find(panel->board, revision, rows[i].card.assignee, &person) == UMI_STATUS_OK)
            g_strlcpy(names[i], person.name, sizeof(names[i]));
        else g_strlcpy(names[i], rows[i].card.assignee, sizeof(names[i]));
    }
    if (status != UMI_STATUS_OK) { g_free(rows); g_free(names); g_object_unref(root); return status; }
    /* Rebuilding emits GTK notifications. Reentrant actions cannot access a
     * half-presented view or start another refresh. */
    panel->refreshing = true;
    panel->drag_card[0] = '\0';
    if(panel->revision!=revision)InvalidatePublic(panel);
    panel->config = config; panel->revision = revision;
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->lanes)) != NULL) gtk_box_remove(GTK_BOX(panel->lanes), child);
    for (size_t col = 0U; panel->board != NULL && col < config.column_count; ++col) {
        GtkWidget *column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_widget_set_size_request(column, 245, 180);
        gtk_widget_set_name(column, config.columns[col].id);
        gtk_widget_set_valign(column, GTK_ALIGN_START); gtk_box_append(GTK_BOX(panel->lanes), column);
        gtk_box_append(GTK_BOX(column), Label(config.columns[col].title));
        GtkDropTarget *drop = gtk_drop_target_new(G_TYPE_STRING, GDK_ACTION_MOVE);
        Bind(drop, "drop", G_CALLBACK(Drop), panel, NULL, config.columns[col].id, revision);
        gtk_widget_add_controller(column, GTK_EVENT_CONTROLLER(drop));
        size_t visible = 0U;
        for (size_t i = 0U; i < count; ++i) if (strcmp(rows[i].column_id, config.columns[col].id) == 0) {
            GtkWidget *button = gtk_button_new(), *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
            gtk_widget_set_name(button, rows[i].card.id);
            gtk_button_set_child(GTK_BUTTON(button), body); gtk_box_append(GTK_BOX(body), Label(rows[i].card.title));
            gtk_box_append(GTK_BOX(body), Label(names[i][0] == '\0' ? "Unassigned" : names[i]));
            /* Card snapshots were copied before publication; display private
             * labels and absolute deadlines as plain text without model reads. */
            if (rows[i].card.label_count != 0U) {
                GString *labels_text = g_string_new("Labels: ");
                for (size_t label = 0U; label < rows[i].card.label_count; ++label) {
                    if (label != 0U) g_string_append(labels_text, " | ");
                    g_string_append(labels_text, rows[i].card.labels[label]);
                }
                GtkWidget *metadata = Label(labels_text->str);
                gtk_widget_set_name(metadata, "kanban-card-labels-summary");
                gtk_box_append(GTK_BOX(body), metadata); g_string_free(labels_text, TRUE);
            }
            if (rows[i].card.due_unix_seconds != 0U) {
                char *date = DeadlineText(rows[i].card.due_unix_seconds);
                char message[128];
                if (date) g_snprintf(message, sizeof message, "Due: %s (UTC)", date);
                else g_snprintf(message, sizeof message, "Due: %" G_GUINT64_FORMAT " UTC seconds (outside date display)", (guint64)rows[i].card.due_unix_seconds);
                g_free(date);
                GtkWidget *metadata = Label(message);
                gtk_widget_set_name(metadata, "kanban-card-deadline-summary");
                gtk_box_append(GTK_BOX(body), metadata);
            }
            gtk_box_append(GTK_BOX(column), button); ++visible;
            Bind(button, "clicked", G_CALLBACK(SelectCard), panel, rows[i].card.id, NULL, revision);
            GtkDragSource *drag = gtk_drag_source_new(); gtk_drag_source_set_actions(drag, GDK_ACTION_MOVE);
            Bind(drag, "prepare", G_CALLBACK(PrepareDrag), panel, rows[i].card.id, NULL, revision);
            Bind(drag, "drag-end", G_CALLBACK(EndDrag), panel, rows[i].card.id, NULL, revision);
            gtk_widget_add_controller(button, GTK_EVENT_CONTROLLER(drag));
        }
        /* The previous empty-column message is retained. A filtered list can
         * have hidden occupancy, so empty presentation does not imply capacity. */
#if 0
        if (visible == 0U) gtk_box_append(GTK_BOX(column), Label("Drop a card here"));
#endif
        if (visible == 0U) gtk_box_append(GTK_BOX(column), Label("No visible cards. Check active occupancy below before moving work here."));
        char occupancy[96];
        /* Keep the former all-visible label for comparison. Filtered rows
         * must never understate the occupancy used by board admission rules. */
#if 0
        if (config.columns[col].wip_limit != 0U)
            g_snprintf(occupancy, sizeof occupancy, "%zu active / limit %zu", visible, config.columns[col].wip_limit);
        else g_snprintf(occupancy, sizeof occupancy, "%zu active / unlimited", visible);
#endif
        if (config.columns[col].wip_limit != 0U)
            g_snprintf(occupancy, sizeof occupancy, "%zu shown / %zu active / limit %zu", visible, summary.columns[col], config.columns[col].wip_limit);
        else g_snprintf(occupancy, sizeof occupancy, "%zu shown / %zu active / unlimited", visible, summary.columns[col]);
        GtkWidget *limit_label = Label(occupancy); gtk_widget_set_name(limit_label, "kanban-list-limit");
        gtk_box_append(GTK_BOX(column), limit_label);
    }
    panel->refreshing = false;
    status = panel->board == NULL ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    g_free(rows); g_free(names); g_object_unref(root); return status;
}
GtkWidget *UmiKanbanGtk4Create(UmiKanbanBoard *board)
{
    if (board == NULL) return NULL;
    Panel *panel = g_new0(Panel, 1); panel->board = board;
    panel->publication = g_new0(UmiKanbanPublicCard, UMI_KANBAN_MAX_CARDS);
    panel->drag_token = g_uuid_string_random(); panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    g_object_set_data_full(G_OBJECT(panel->root), PANEL_KEY, panel, FreePanel);
    panel->status = Label("Select a card, type a developer name, or drag cards between lists. Save changes through the host.");
    gtk_widget_set_name(panel->status, "kanban-status");
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    panel->title = Entry(panel->root, "Card title", 255);
    gtk_widget_set_name(panel->title, "kanban-title");
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(panel->root), actions);
    Button(panel, actions, "Add card", G_CALLBACK(AddCard)); Button(panel, actions, "Rename selected", G_CALLBACK(RenameCard)); Button(panel, actions, "Refresh", G_CALLBACK(RefreshClick));
    panel->developer = Entry(panel->root, "Developer name (empty clears assignment)", 127);
    gtk_widget_set_name(panel->developer, "kanban-developer");
    panel->suggestions = Label("Type a name to find existing developers."); gtk_box_append(GTK_BOX(panel->root), panel->suggestions);
    Bind(panel->developer, "changed", G_CALLBACK(Suggest), panel, NULL, NULL, 0U);
    Bind(panel->developer, "activate", G_CALLBACK(AssignName), panel, NULL, NULL, 0U);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(panel->root), actions);
    Button(panel, actions, "Assign name", G_CALLBACK(AssignName));
    UmiKanbanConfig config; if (umi_kanban_board_config(board, &config) != UMI_STATUS_OK) { g_object_ref_sink(panel->root); g_object_unref(panel->root); return NULL; }
    GtkStringList *destinations = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < config.column_count; ++i) gtk_string_list_append(destinations, g_utf8_validate(config.columns[i].title, -1, NULL) ? config.columns[i].title : config.columns[i].id);
/* Transfer the model through the Framework helper so the ownership contract is explicit and shared. The previous statement is retained for review. */
#if 0
    panel->destination = gtk_drop_down_new(G_LIST_MODEL(destinations), NULL);
#endif
    panel->destination = umi_ui_gtk4_drop_down_new_take_string_list(destinations);
    gtk_widget_set_name(panel->destination, "kanban-destination");
    gtk_box_append(GTK_BOX(actions), panel->destination); Button(panel, actions, "Move selected", G_CALLBACK(MoveSelected));
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_widget_set_vexpand(scroll, TRUE); gtk_widget_set_size_request(scroll, -1, 260);
    panel->lanes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->lanes); gtk_box_append(GTK_BOX(panel->root), scroll);
    GtkWidget *filter = gtk_expander_new("Filter board cards");
    gtk_widget_set_name(filter, "kanban-view-filter-expander");
    gtk_expander_set_child(GTK_EXPANDER(filter), CreateFilterForm(panel));
    gtk_box_append(GTK_BOX(panel->root), filter);
    GtkWidget *details = gtk_expander_new("Edit private card details");
    gtk_widget_set_name(details, "kanban-card-details-expander");
    gtk_expander_set_child(GTK_EXPANDER(details), CreateDetailsForm(panel));
    gtk_box_append(GTK_BOX(panel->root), details);
    GtkWidget *checklist = gtk_expander_new("Edit acceptance checklist");
    gtk_widget_set_name(checklist, "kanban-card-checklist-expander");
    gtk_expander_set_child(GTK_EXPANDER(checklist), CreateChecklistForm(panel));
    gtk_box_append(GTK_BOX(panel->root), checklist);
    GtkWidget *dependencies = gtk_expander_new("Edit prerequisite cards");
    gtk_widget_set_name(dependencies, "kanban-card-dependencies-expander");
    gtk_expander_set_child(GTK_EXPANDER(dependencies), CreateDependenciesForm(panel));
    gtk_box_append(GTK_BOX(panel->root), dependencies);
    GtkWidget *labels = gtk_expander_new("Edit private card labels");
    gtk_widget_set_name(labels, "kanban-card-labels-expander");
    gtk_expander_set_child(GTK_EXPANDER(labels), CreateLabelsForm(panel));
    gtk_box_append(GTK_BOX(panel->root), labels);
    GtkWidget *deadline = gtk_expander_new("Edit card deadline in UTC");
    gtk_widget_set_name(deadline, "kanban-card-deadline-expander");
    gtk_expander_set_child(GTK_EXPANDER(deadline), CreateDeadlineForm(panel));
    gtk_box_append(GTK_BOX(panel->root), deadline);
    GtkWidget *archive = gtk_expander_new("Archive or restore cards");
    gtk_widget_set_name(archive, "kanban-card-archive-expander");
    gtk_expander_set_child(GTK_EXPANDER(archive), CreateArchiveForm(panel));
    gtk_box_append(GTK_BOX(panel->root), archive);
    GtkWidget *policy = gtk_expander_new("Review list work limits");
    gtk_widget_set_name(policy, "kanban-column-policy-expander");
    gtk_expander_set_child(GTK_EXPANDER(policy), CreatePolicyForm(panel, &config));
    gtk_box_append(GTK_BOX(panel->root), policy);
    /* Private completion review is separate from the public website fields.
     * The first Done-stage column is shown in the retained decision preview. */
    panel->completion_review = CreateCompletionReview(panel);
    if (panel->completion_review != NULL) {
        g_object_ref_sink(panel->completion_review);
        GtkWidget *review = gtk_expander_new("Review evidence before completing cards");
        gtk_widget_set_name(review, "kanban-completion-review");
        gtk_expander_set_child(GTK_EXPANDER(review), panel->completion_review);
        gtk_box_append(GTK_BOX(panel->root), review);
    }
    GtkWidget *expander = gtk_expander_new("Review a public website snapshot"), *publication = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_name(expander, "kanban-publication-expander");
    gtk_expander_set_child(GTK_EXPANDER(expander), publication); gtk_box_append(GTK_BOX(panel->root), expander);
    /* The original temporary-selection guidance is retained for review.
     * Explicit private plan persistence now survives application restart. */
#if 0
    gtk_box_append(GTK_BOX(publication), Label("Only the public fields you enter below are exported. Source descriptions, activity, identifiers and paths stay private. The selection is temporary until exported."));
#endif
    gtk_box_append(GTK_BOX(publication), Label("Only reviewed public fields are exported. Private descriptions, activity, identifiers and paths stay private. Save a private selection plan to retain the review; regenerate website HTML after changing it."));
    panel->site_title = Entry(publication, "Public board title", 255); gtk_widget_set_name(panel->site_title,"kanban-public-board-title"); gtk_editable_set_text(GTK_EDITABLE(panel->site_title), "Umicom contributor board");
    panel->public_title = Entry(publication, "Public title for the selected card", 255);
    gtk_widget_set_name(panel->public_title, "kanban-public-title");
    panel->public_summary = Entry(publication, "Public summary (optional)", 1023);
    panel->public_developer = Entry(publication, "Developer name approved for publication (optional)", 127);
    actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(publication), actions);
    Button(panel, actions, "Include reviewed card", G_CALLBACK(IncludePublic)); Button(panel, actions, "Clear selection", G_CALLBACK(ClearPublic)); Button(panel, actions, "Generate website", G_CALLBACK(GeneratePublic));
    GtkWidget *source = gtk_text_view_new(); gtk_widget_set_name(source, "kanban-public-source"); gtk_text_view_set_editable(GTK_TEXT_VIEW(source), FALSE); gtk_text_view_set_monospace(GTK_TEXT_VIEW(source), TRUE); panel->html = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
    GtkWidget *source_scroll = gtk_scrolled_window_new(); gtk_widget_set_size_request(source_scroll, -1, 140); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(source_scroll), source); gtk_box_append(GTK_BOX(publication), source_scroll);
    /* Keep the old unguarded buffer export for review. Publication now validates
     * its board and reviewed-field generations immediately before capture. */
#if 0
    panel->export_control = UmiTextExportGtk4CreateForBuffer(panel->html, "index.html");
#endif
    panel->public_generation=1U;
    Bind(panel->site_title,"changed",G_CALLBACK(PublicTitleChanged),panel,NULL,NULL,0U);
    panel->export_control=CreateReviewedExport(panel);
    if (panel->export_control != NULL) {
        /* The panel owns a reference as well as the parent; finalization may
         * dispose GTK children before the panel data destructor runs. */
        g_object_ref_sink(panel->export_control);
        gtk_box_append(GTK_BOX(publication), panel->export_control);
    }
    panel->public_plan_store=CreatePublicPlanStore(panel);
    if(panel->public_plan_store!=NULL){
        g_object_ref_sink(panel->public_plan_store);
        GtkWidget *saved=gtk_expander_new("Save or reopen a private public-selection plan");
        gtk_expander_set_child(GTK_EXPANDER(saved),panel->public_plan_store);
        gtk_box_append(GTK_BOX(publication),saved);
    }
    if (UmiKanbanGtk4Refresh(panel->root) != UMI_STATUS_OK) { GtkWidget *root = panel->root; g_object_ref_sink(root); g_object_unref(root); return NULL; }
    return panel->root;
}
/* The previous transition order is retained for review. The replacement
 * retains the controller through focus notifications and finishes native text
 * input before disabling its subtree; model retirement remains irreversible. */
#if 0
void UmiKanbanGtk4SetBusy(GtkWidget *root, bool busy)
{
    if (!GTK_IS_WIDGET(root)) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel == NULL || panel->board == NULL) return;
    UmiKanbanPublicPlanGtk4SetBusy(panel->public_plan_store,busy);
    panel->busy = busy; panel->drag_card[0] = '\0'; gtk_widget_set_sensitive(root, !busy);
}
void UmiKanbanGtk4Retire(GtkWidget *root)
{
    if (!GTK_IS_WIDGET(root)) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel == NULL) return;
    panel->board = NULL; panel->busy = true; panel->drag_card[0] = '\0';
    UmiKanbanPublicPlanGtk4Retire(panel->public_plan_store);
    UmiTextExportGtk4Retire(panel->export_control); gtk_widget_set_sensitive(root, FALSE);
}
#endif
void UmiKanbanGtk4SetBusy(GtkWidget *root, bool busy)
{
    if (!GTK_IS_WIDGET(root)) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel == NULL || panel->board == NULL) return;
    g_object_ref(root);
    /* Guard borrowed model access before focus notifications. A reentrant
     * retirement takes precedence over this request to enable the controls. */
    panel->busy = busy;
    if (busy) {
        panel->details_ready = false; panel->details_interrupted = true;
        panel->checklist_ready = false; panel->checklist_interrupted = true;
        panel->dependencies_ready = false; panel->dependencies_interrupted = true;
        panel->labels_ready = false; panel->labels_interrupted = true;
        panel->policy_ready = false; panel->policy_interrupted = true;
        panel->deadline_ready = false; panel->deadline_interrupted = true;
        panel->archive_ready = false; panel->archive_interrupted = true;
    }
    panel->drag_card[0] = '\0';
    UmiKanbanPublicPlanGtk4SetBusy(panel->public_plan_store, busy);
    UmiKanbanCompletionGtk4SetBusy(panel->completion_review, busy);
    if (busy) UmiGtk4ReleaseSubtreeFocus(root);
    gtk_widget_set_sensitive(root, panel->board != NULL && !panel->busy);
    g_object_unref(root);
}
void UmiKanbanGtk4Retire(GtkWidget *root)
{
    if (!GTK_IS_WIDGET(root)) return;
    Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
    if (panel == NULL || panel->board == NULL) return;
    g_object_ref(root);
    panel->board = NULL;
    panel->busy = true;
    panel->details_ready = false; panel->details_interrupted = true;
    panel->checklist_ready = false; panel->checklist_interrupted = true;
    panel->dependencies_ready = false; panel->dependencies_interrupted = true;
    panel->labels_ready = false; panel->labels_interrupted = true;
    panel->policy_ready = false; panel->policy_interrupted = true;
    panel->deadline_ready = false; panel->deadline_interrupted = true;
    panel->archive_ready = false; panel->archive_interrupted = true;
    panel->drag_card[0] = '\0';
    UmiGtk4ReleaseSubtreeFocus(root);
    UmiKanbanPublicPlanGtk4Retire(panel->public_plan_store);
    UmiKanbanCompletionGtk4Retire(panel->completion_review);
    UmiTextExportGtk4Retire(panel->export_control);
    gtk_widget_set_sensitive(root, FALSE);
    g_object_unref(root);
}
