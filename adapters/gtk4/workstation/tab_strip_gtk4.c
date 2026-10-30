/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/tab_strip_gtk4.c
 *
 * PURPOSE:
 *   Reconcile native tab buttons by identity without cancelling focus or input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/tab_strip.h"
#include <string.h>

#define STRIP_STATE "umicom-tab-strip-state"
#define TAB_ID "umicom-tab-id"
typedef struct TabStripState {
    GtkWidget *box;
    GtkWidget *empty;
    UmiGtk4TabStripActivate activate;
    void *context;
    GDestroyNotify release_context;
    int alive;
    int updating;
} TabStripState;
typedef struct TabButtonBinding { GWeakRef strip; } TabButtonBinding;

static TabStripState *StripState(GtkWidget *strip)
{
    return GTK_IS_SCROLLED_WINDOW(strip) ? g_object_get_data(G_OBJECT(strip), STRIP_STATE) : NULL;
}
static int StripOwned(GtkWidget *strip, TabStripState *state)
{
    /* Both owned children must remain attached before reconciliation can reorder
     * buttons. A notification may otherwise detach the empty-state anchor. */
    return state->alive && gtk_widget_is_ancestor(state->box, strip) &&
        gtk_widget_get_parent(state->empty) == state->box;
}
static void StripFree(gpointer data)
{
    TabStripState *state = data;
    if (state->release_context != NULL) state->release_context(state->context);
    g_object_unref(state->empty);
    g_object_unref(state->box);
    g_free(state);
}
static void ButtonFree(gpointer data, GClosure *closure)
{
    (void)closure;
    TabButtonBinding *binding = data;
    g_weak_ref_clear(&binding->strip);
    g_free(binding);
}
static void ButtonActivate(GtkButton *button, gpointer data)
{
    TabButtonBinding *binding = data;
    GtkWidget *strip = g_weak_ref_get(&binding->strip);
    if (strip == NULL) return;
    TabStripState *state = StripState(strip);
    if (state != NULL && StripOwned(strip, state) && !state->updating &&
        gtk_widget_get_parent(GTK_WIDGET(button)) == state->box &&
        gtk_widget_is_sensitive(GTK_WIDGET(button)) && state->activate != NULL) {
        /* The callback may remove the button or retire the host. Keep both
         * context ownership and the selected identity alive through dispatch. */
        char *id = g_strdup(g_object_get_data(G_OBJECT(button), TAB_ID));
        state->activate(id, state->context);
        g_free(id);
    }
    g_object_unref(strip);
}
GtkWidget *UmiGtk4TabStripCreate(UmiGtk4TabStripActivate activate,
    void *context, GDestroyNotify release_context)
{
    GtkWidget *strip = gtk_scrolled_window_new();
    TabStripState *state = g_new0(TabStripState, 1);
    state->box = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3));
    state->empty = g_object_ref_sink(gtk_label_new("No open documents"));
    state->activate = activate;
    state->context = context;
    state->release_context = release_context;
    state->alive = 1;
    gtk_widget_add_css_class(state->empty, "dim-label");
    gtk_box_append(GTK_BOX(state->box), state->empty);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(strip), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(strip), TRUE);
    gtk_widget_set_hexpand(strip, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(strip), state->box);
    g_object_set_data_full(G_OBJECT(strip), STRIP_STATE, state, StripFree);
    return strip;
}
GtkWidget *UmiGtk4TabStripFind(GtkWidget *strip, const char *id)
{
    TabStripState *state = StripState(strip);
    if (state == NULL || !StripOwned(strip, state) || id == NULL) return NULL;
    for (GtkWidget *child = gtk_widget_get_first_child(state->box); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        if (GTK_IS_BUTTON(child) && g_strcmp0(g_object_get_data(G_OBJECT(child), TAB_ID), id) == 0)
            return child;
    return NULL;
}
void UmiGtk4TabStripInvalidate(GtkWidget *strip)
{
    TabStripState *state = StripState(strip);
    if (state != NULL) state->alive = 0;
}
static int ValidText(const char *text, size_t limit, int nonempty)
{
    if (text == NULL || (nonempty && text[0] == '\0')) return 0;
    size_t length = 0U;
    while (length <= limit && text[length] != '\0') ++length;
    return length <= limit && g_utf8_validate(text, (gssize)length, NULL);
}
static int HasId(const UmiGtk4TabStripItem *items, size_t count, const char *id)
{
    if (id == NULL) return 0;
    for (size_t index = 0U; index < count; ++index)
        if (strcmp(items[index].id, id) == 0) return 1;
    return 0;
}
UmiStatus UmiGtk4TabStripSync(GtkWidget *strip,
    const UmiGtk4TabStripItem *items, size_t count)
{
    TabStripState *state = StripState(strip);
    if (state == NULL || (count != 0U && items == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!StripOwned(strip, state)) return UMI_STATUS_CANCELLED;
    if (state->updating) return UMI_STATUS_BUSY;
    if (count > UMI_GTK4_TAB_STRIP_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t selected = 0U;
    for (size_t index = 0U; index < count; ++index) {
        if (!ValidText(items[index].id, 255U, 1) || !ValidText(items[index].title, 511U, 1) ||
            (items[index].tooltip != NULL && !ValidText(items[index].tooltip, 4095U, 0)) ||
            HasId(items, index, items[index].id)) return UMI_STATUS_INVALID_ARGUMENT;
        selected += items[index].selected != 0 ? 1U : 0U;
    }
    if (selected > 1U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Capture caller-owned strings before GTK notifications can release them.
     * Routine refresh changes labels/styles only when their values differ. */
    UmiGtk4TabStripItem *copy = g_new0(UmiGtk4TabStripItem, count);
    for (size_t index = 0U; index < count; ++index) {
        copy[index] = items[index];
        copy[index].id = g_strdup(items[index].id);
        copy[index].title = g_strdup(items[index].title);
        copy[index].tooltip = g_strdup(items[index].tooltip);
    }
    g_object_ref(strip);
    state->updating = 1;
    UmiStatus status = UMI_STATUS_OK;
    GtkWidget *held = NULL;
    /* Snapshot references keep removal iteration valid if an observer changes
     * siblings. A detached old button cannot dispatch through ButtonActivate. */
    GPtrArray *children = g_ptr_array_new_with_free_func(g_object_unref);
    for (GtkWidget *child = gtk_widget_get_first_child(state->box); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        g_ptr_array_add(children, g_object_ref(child));
    for (guint index = 0U; index < children->len; ++index) {
        GtkWidget *child = g_ptr_array_index(children, index);
        if (!StripOwned(strip, state)) { status = UMI_STATUS_CANCELLED; goto done; }
        if (child != state->empty && gtk_widget_get_parent(child) == state->box &&
            !HasId(copy, count, g_object_get_data(G_OBJECT(child), TAB_ID)))
            gtk_box_remove(GTK_BOX(state->box), child);
    }
    for (size_t index = 0U; index < count; ++index) {
        if (!StripOwned(strip, state)) { status = UMI_STATUS_CANCELLED; goto done; }
        GtkWidget *button = UmiGtk4TabStripFind(strip, copy[index].id);
        if (button == NULL) {
            button = gtk_button_new_with_label(copy[index].title);
            g_object_set_data_full(G_OBJECT(button), TAB_ID, g_strdup(copy[index].id), g_free);
            TabButtonBinding *binding = g_new0(TabButtonBinding, 1);
            g_weak_ref_init(&binding->strip, strip);
            g_signal_connect_data(button, "clicked", G_CALLBACK(ButtonActivate), binding, ButtonFree, 0);
            GtkWidget *label = gtk_button_get_child(GTK_BUTTON(button));
            gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
            gtk_label_set_max_width_chars(GTK_LABEL(label), 32);
            held = g_object_ref_sink(button);
            gtk_box_append(GTK_BOX(state->box), button);
        } else held = g_object_ref(button);
#define REQUIRE_OWNED() do { if (!StripOwned(strip, state) ||     gtk_widget_get_parent(button) != state->box) { status = UMI_STATUS_CANCELLED; goto done; } } while (0)
        REQUIRE_OWNED();
        if (g_strcmp0(gtk_button_get_label(GTK_BUTTON(button)), copy[index].title) != 0)
            gtk_button_set_label(GTK_BUTTON(button), copy[index].title);
        REQUIRE_OWNED();
        if (g_strcmp0(gtk_widget_get_tooltip_text(button), copy[index].tooltip) != 0)
            gtk_widget_set_tooltip_text(button, copy[index].tooltip);
        REQUIRE_OWNED();
        if (copy[index].selected && !gtk_widget_has_css_class(button, "suggested-action"))
            gtk_widget_add_css_class(button, "suggested-action");
        else if (!copy[index].selected && gtk_widget_has_css_class(button, "suggested-action"))
            gtk_widget_remove_css_class(button, "suggested-action");
        REQUIRE_OWNED();
        GtkWidget *previous = index == 0U ? state->empty : UmiGtk4TabStripFind(strip, copy[index - 1U].id);
        if (previous == NULL) { status = UMI_STATUS_CANCELLED; goto done; }
        if (gtk_widget_get_prev_sibling(button) != previous)
            gtk_box_reorder_child_after(GTK_BOX(state->box), button, previous);
        REQUIRE_OWNED();
        g_clear_object(&held);
#undef REQUIRE_OWNED
    }
    if (!StripOwned(strip, state) || gtk_widget_get_parent(state->empty) != state->box) {
        status = UMI_STATUS_CANCELLED; goto done;
    }
    gtk_widget_set_visible(state->empty, count == 0U);
    if (!StripOwned(strip, state)) status = UMI_STATUS_CANCELLED;
done:
    g_clear_object(&held);
    g_ptr_array_unref(children);
    for (size_t index = 0U; index < count; ++index) {
        g_free((void *)copy[index].id); g_free((void *)copy[index].title);
        g_free((void *)copy[index].tooltip);
    }
    g_free(copy);
    state->updating = 0;
    g_object_unref(strip);
    return status;
}
