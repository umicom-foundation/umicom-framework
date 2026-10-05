/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/filtered_choices.c
 * PURPOSE: Present owned filtered snapshots without reassigning source row identities.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/filtered_choices.h"
#include "umicom/ui/text_projection.h"
#include <string.h>

#define CHOICES_OWNER "umicom-filtered-choices-owner"
typedef struct FilteredChoices
{
    GtkDropDown *picker;
    GtkWidget *query, *order, *match_case, *count;
    GtkStringList *visible;
    UmiUiTextProjection *projection;
    UmiUiSortFilterSnapshot applied;
    bool sort, busy;
} FilteredChoices;

static FilteredChoices *ChoicesOwner(GtkWidget *controls)
{
    return GTK_IS_WIDGET(controls) ? g_object_get_data(G_OBJECT(controls), CHOICES_OWNER) : NULL;
}
static void ChoicesFree(gpointer data)
{
    FilteredChoices *state = data;
    UmiUiTextProjectionDestroy(state->projection);
    g_clear_object(&state->visible);
    g_clear_object(&state->picker);
    g_clear_object(&state->query);
    g_clear_object(&state->order);
    g_clear_object(&state->match_case);
    g_clear_object(&state->count);
    g_free(state);
}
UmiStatus UmiGtk4FilteredChoicesSelectedSource(GtkWidget *controls, size_t *out_source)
{
    FilteredChoices *state = ChoicesOwner(controls);
    if (state == NULL || out_source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->busy)
        return UMI_STATUS_BUSY;
    if (gtk_drop_down_get_model(state->picker) != G_LIST_MODEL(state->visible))
        return UMI_STATUS_INVALID_STATE;
    if (state->projection == NULL)
        return UMI_STATUS_NOT_FOUND;
    return UmiUiTextProjectionToSource(state->projection, (size_t)gtk_drop_down_get_selected(state->picker),
                                       out_source);
}
/* Every mutation below may notify application code. The public caller retains
 * the root and busy blocks nested publication or selection until both the
 * mapping and the native list agree. The final selected notification is last:
 * observers may replace data or dispose their dialog from that callback. */
static void ChoicesPublish(FilteredChoices *state, size_t previous_source)
{
    size_t count = UmiUiTextProjectionCount(state->projection), selected = SIZE_MAX;
    GtkStringList *visible = count != 0U ? gtk_string_list_new(NULL) : NULL;
    for (size_t i = 0U; i < count; ++i)
    {
        size_t source = 0U;
        (void)UmiUiTextProjectionToSource(state->projection, i, &source);
        gtk_string_list_append(visible, UmiUiTextProjectionText(state->projection, source));
    }
    if (previous_source != SIZE_MAX)
        (void)UmiUiTextProjectionToView(state->projection, previous_source, &selected);
    g_clear_object(&state->visible);
    state->visible = visible;
    gtk_drop_down_set_model(state->picker, G_LIST_MODEL(visible));
    gtk_drop_down_set_selected(state->picker,
                               selected == SIZE_MAX ? GTK_INVALID_LIST_POSITION : (guint)selected);
    char *summary =
        g_strdup_printf("%zu of %zu rows | Applied text: %s | %s | %s", count,
                        UmiUiTextProjectionSourceCount(state->projection),
                        state->applied.query[0] ? state->applied.query : "(all)",
                        state->sort ? (state->applied.ascending ? "A-Z" : "Z-A") : "original order",
                        state->applied.case_sensitive ? "match case" : "ignore ASCII case");
    gtk_label_set_text(GTK_LABEL(state->count), summary);
    g_free(summary);
    state->busy = false;
    g_object_notify(G_OBJECT(state->picker), "selected");
}
UmiStatus UmiGtk4FilteredChoicesSetRows(GtkWidget *controls, const char *const *rows, size_t count)
{
    FilteredChoices *state = ChoicesOwner(controls);
    if (state == NULL || (count != 0U && rows == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (state->busy)
        return UMI_STATUS_BUSY;
    if (count > UMI_UI_TEXT_PROJECTION_ROWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Validate bounded strings before giving them to GTK; the portable model
     * deliberately remains useful for byte-oriented non-GTK consumers. */
    for (size_t i = 0U; i < count; ++i)
    {
        if (rows[i] == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
        size_t bytes = 0U;
        while (bytes < UMI_UI_TEXT_PROJECTION_ROW_BYTES && rows[i][bytes] != '\0')
            ++bytes;
        if (bytes == UMI_UI_TEXT_PROJECTION_ROW_BYTES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (!g_utf8_validate(rows[i], (gssize)bytes, NULL))
            return UMI_STATUS_PARSE_ERROR;
    }
    UmiUiTextProjection *next = NULL;
    UmiStatus status = UmiUiTextProjectionCreate(rows, count, &next);
    if (status == UMI_STATUS_OK)
        status = UmiUiTextProjectionApply(next, &state->applied, state->sort);
    if (status != UMI_STATUS_OK)
    {
        UmiUiTextProjectionDestroy(next);
        return status;
    }
    g_object_ref(controls);
    state->busy = true;
    UmiUiTextProjectionDestroy(state->projection);
    state->projection = next;
    ChoicesPublish(state, SIZE_MAX);
    g_object_unref(controls);
    return UMI_STATUS_OK;
}
static void ChoicesApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *controls = g_object_ref(data);
    FilteredChoices *state = ChoicesOwner(controls);
    if (state == NULL || state->busy)
    {
        g_object_unref(controls);
        return;
    }
    size_t previous = SIZE_MAX;
    (void)UmiGtk4FilteredChoicesSelectedSource(controls, &previous);
    UmiUiSortFilterSnapshot filter = {0};
    const char *query = gtk_editable_get_text(GTK_EDITABLE(state->query));
    if (strlen(query) >= sizeof(filter.query))
    {
        gtk_label_set_text(
            GTK_LABEL(state->count),
            "Filter not applied: use at most 255 UTF-8 bytes. The previous list remains active.");
        g_object_unref(controls);
        return;
    }
    memcpy(filter.query, query, strlen(query) + 1U);
    filter.enabled = 1;
    filter.case_sensitive = gtk_check_button_get_active(GTK_CHECK_BUTTON(state->match_case));
    guint order = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->order));
    filter.ascending = order != 2U;
    bool sort = order != 0U;
    state->busy = true;
    if (state->projection != NULL)
    {
        (void)UmiUiTextProjectionApply(state->projection, &filter, sort);
        state->applied = filter;
        state->sort = sort;
        ChoicesPublish(state, previous);
    }
    else
        state->busy = false;
    g_object_unref(controls);
}
static void ChoicesTag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
    gtk_widget_set_name(widget, id);
}
GtkWidget *UmiGtk4FilteredChoicesCreate(GtkDropDown *picker)
{
    if (!GTK_IS_DROP_DOWN(picker))
        return NULL;
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    FilteredChoices *state = g_new0(FilteredChoices, 1);
    state->picker = g_object_ref(picker);
    state->applied.enabled = 1;
    state->applied.ascending = 1;
    state->query = g_object_ref_sink(gtk_entry_new());
    gtk_entry_set_placeholder_text(GTK_ENTRY(state->query), "Literal text to find in these rows");
    gtk_entry_set_max_length(GTK_ENTRY(state->query), 255);
    const char *orders[] = {"Original order", "Text A-Z", "Text Z-A", NULL};
    state->order = g_object_ref_sink(gtk_drop_down_new_from_strings(orders));
    state->match_case = g_object_ref_sink(gtk_check_button_new_with_label("Match case"));
    state->count = g_object_ref_sink(gtk_label_new("No rows captured."));
    gtk_label_set_wrap(GTK_LABEL(state->count), TRUE);
    gtk_label_set_xalign(GTK_LABEL(state->count), 0.0F);
    GtkWidget *apply = gtk_button_new_with_label("Apply filter and order");
    GtkWidget *items[] = {state->query, state->order, state->match_case, apply, state->count};
    const char *ids[] = {"choices.query", "choices.order", "choices.case", "choices.apply", "choices.count"};
    for (size_t i = 0U; i < G_N_ELEMENTS(items); ++i)
    {
        ChoicesTag(items[i], ids[i]);
        gtk_box_append(GTK_BOX(root), items[i]);
    }
    g_object_set_data_full(G_OBJECT(root), CHOICES_OWNER, state, ChoicesFree);
    /* Object-bound signals disconnect if callers retain a child control after
     * the root dies. They never keep a dead application context alive. */
    g_signal_connect_object(apply, "clicked", G_CALLBACK(ChoicesApply), root, 0);
    return root;
}
