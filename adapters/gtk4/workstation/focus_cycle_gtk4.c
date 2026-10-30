/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/focus_cycle_gtk4.c
 *
 * PURPOSE:
 *   Move keyboard focus across live panels without rebuilding their content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/focus_cycle.h"

static const char RegionKey[] = "umicom-focus-cycle-region";
static const char InstalledKey[] = "umicom-focus-cycle-installed";
static const char BusyKey[] = "umicom-focus-cycle-busy";

static gboolean Within(GtkWidget *widget, GtkWidget *container)
{
    return widget != NULL && (widget == container ||
        gtk_widget_is_ancestor(widget, container));
}

static gboolean Available(GtkWidget *widget)
{
    return gtk_widget_get_mapped(widget) && gtk_widget_is_visible(widget) &&
        gtk_widget_is_sensitive(widget) && gtk_widget_get_can_focus(widget);
}

/* Snapshot owned references before notifying GTK focus operations. No callback
 * is invoked during this walk. Iteration avoids a recursive C stack for deeply
 * nested provider widgets. Nested workspaces own their own unmarked children. */
static GPtrArray *Regions(GtkWidget *workspace)
{
    GPtrArray *regions = g_ptr_array_new_with_free_func(g_object_unref);
    GtkWidget *widget = gtk_widget_get_first_child(workspace);
    while (widget != NULL) {
        gboolean eligible = Available(widget);
        gboolean marked = g_object_get_data(G_OBJECT(widget), RegionKey) != NULL;
        gboolean nested = g_object_get_data(G_OBJECT(widget), InstalledKey) != NULL;
        if (eligible && marked) g_ptr_array_add(regions, g_object_ref(widget));
        if (eligible && !marked && !nested && gtk_widget_get_first_child(widget) != NULL) {
            widget = gtk_widget_get_first_child(widget);
            continue;
        }
        while (widget != workspace && gtk_widget_get_next_sibling(widget) == NULL)
            widget = gtk_widget_get_parent(widget);
        widget = widget == workspace ? NULL : gtk_widget_get_next_sibling(widget);
    }
    return regions;
}

void UmiGtk4FocusCycleMarkRegion(GtkWidget *region, gboolean enabled)
{
    if (region == NULL) return;
    g_object_set_data(G_OBJECT(region), RegionKey, enabled ? GINT_TO_POINTER(1) : NULL);
}

gboolean UmiGtk4FocusCycleAdvance(GtkWidget *workspace, gboolean reverse)
{
    if (workspace == NULL || !Available(workspace)) return FALSE;
    GtkRoot *native = gtk_widget_get_root(workspace);
    if (native == NULL || g_object_get_data(G_OBJECT(native), BusyKey) != NULL) return FALSE;
    /* Guard the native root: a focus observer may call a different workspace
     * in this window, as well as recursively call this one. */
    g_object_ref(workspace);
    g_object_ref(native);
    g_object_set_data(G_OBJECT(native), BusyKey, GINT_TO_POINTER(1));
    GPtrArray *regions = Regions(workspace);
    GtkWidget *original = gtk_root_get_focus(native);
    if (original != NULL) g_object_ref(original);
    guint current = regions->len;
    gboolean moved = FALSE;
    for (guint i = 0U; i < regions->len; ++i) {
        if (Within(original, g_ptr_array_index(regions, i))) { current = i; break; }
    }
    guint next = current;
    for (guint visited = 0U; visited < regions->len; ++visited) {
        if (next == regions->len) next = reverse ? regions->len - 1U : 0U;
        else if (reverse) next = next == 0U ? regions->len - 1U : next - 1U;
        else next = next + 1U == regions->len ? 0U : next + 1U;
        if (next == current) break;
        if (!Available(workspace) || gtk_widget_get_root(workspace) != native) break;
        GtkWidget *region = g_ptr_array_index(regions, next);
        if (!Within(region, workspace) || !Available(region) ||
            gtk_widget_get_root(region) != native ||
            g_object_get_data(G_OBJECT(region), RegionKey) == NULL) continue;
        (void)gtk_widget_child_focus(region,
            reverse ? GTK_DIR_TAB_BACKWARD : GTK_DIR_TAB_FORWARD);
        /* A GTK return is not enough: a focus observer may have hidden,
         * detached or destroyed the target, or selected a different panel. */
        GtkWidget *focus = gtk_root_get_focus(native);
        gboolean live = Available(workspace) && gtk_widget_get_root(workspace) == native &&
            Within(region, workspace) && Available(region) &&
            gtk_widget_get_root(region) == native &&
            g_object_get_data(G_OBJECT(region), RegionKey) != NULL;
        moved = live && Within(focus, region) && Available(focus);
        if (!live || moved || focus != original) break;
    }
    if (original != NULL) g_object_unref(original);
    g_ptr_array_unref(regions);
    g_object_set_data(G_OBJECT(native), BusyKey, NULL);
    g_object_unref(native);
    g_object_unref(workspace);
    return moved;
}

static gboolean KeyPressed(GtkEventControllerKey *controller, guint key,
    guint keycode, GdkModifierType state, gpointer context)
{
    (void)keycode;
    (void)context;
    const GdkModifierType commands = GDK_CONTROL_MASK | GDK_ALT_MASK |
        GDK_SUPER_MASK | GDK_HYPER_MASK | GDK_META_MASK;
    if (key != GDK_KEY_F6 || (state & commands) != 0) return FALSE;
    GtkWidget *workspace = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
    return UmiGtk4FocusCycleAdvance(workspace, (state & GDK_SHIFT_MASK) != 0);
}

/* GtkPaned owns an F6 keybinding before events reach an outer bubble
 * controller. Route that existing action through the nearest installed host,
 * without replacing resize keys or shortcuts handled inside a panel body. */
static gboolean PanedCycle(GtkPaned *paned, gboolean reverse, gpointer context)
{
    (void)context;
    GtkWidget *pane = GTK_WIDGET(paned);
    GtkWidget *workspace = gtk_widget_get_parent(pane);
    while (workspace != NULL &&
           g_object_get_data(G_OBJECT(workspace), InstalledKey) == NULL)
        workspace = gtk_widget_get_parent(workspace);
    if (workspace == NULL || !Available(workspace)) return FALSE;
    GtkRoot *native = gtk_widget_get_root(workspace);
    if (native == NULL || !Within(gtk_root_get_focus(native), pane)) return FALSE;
    /* Hold the signal emitter because a focus observer can retire its tree.
     * The recognised action is consumed even when no destination is available;
     * falling through would run GTK's second, unrelated traversal afterwards. */
    g_object_ref(paned);
    g_signal_stop_emission_by_name(paned, "cycle-child-focus");
    (void)UmiGtk4FocusCycleAdvance(workspace, reverse);
    g_object_unref(paned);
    return TRUE;
}

static void BindLayoutPanes(GtkWidget *workspace)
{
    if (workspace == NULL) return;
    GtkWidget *widget = gtk_widget_get_first_child(workspace);
    while (widget != NULL) {
        gboolean boundary = g_object_get_data(G_OBJECT(widget), RegionKey) != NULL ||
            g_object_get_data(G_OBJECT(widget), InstalledKey) != NULL;
        if (!boundary && GTK_IS_PANED(widget) &&
            g_object_get_data(G_OBJECT(widget), "umicom-focus-cycle-paned") == NULL) {
            g_signal_connect(widget, "cycle-child-focus", G_CALLBACK(PanedCycle), NULL);
            g_object_set_data(G_OBJECT(widget), "umicom-focus-cycle-paned", GINT_TO_POINTER(1));
        }
        if (!boundary && gtk_widget_get_first_child(widget) != NULL) {
            widget = gtk_widget_get_first_child(widget);
            continue;
        }
        while (widget != workspace && gtk_widget_get_next_sibling(widget) == NULL)
            widget = gtk_widget_get_parent(widget);
        widget = widget == workspace ? NULL : gtk_widget_get_next_sibling(widget);
    }
}

void UmiGtk4FocusCycleInstall(GtkWidget *workspace)
{
    BindLayoutPanes(workspace);
    if (workspace == NULL || g_object_get_data(G_OBJECT(workspace), InstalledKey) != NULL) return;
    GtkEventController *controller = gtk_event_controller_key_new();
    gtk_event_controller_set_name(controller, "umicom-workspace-focus-cycle");
    gtk_event_controller_set_propagation_phase(controller, GTK_PHASE_BUBBLE);
    g_signal_connect(controller, "key-pressed", G_CALLBACK(KeyPressed), NULL);
    gtk_widget_add_controller(workspace, controller);
    g_object_set_data(G_OBJECT(workspace), InstalledKey, GINT_TO_POINTER(1));
}
