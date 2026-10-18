/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/widget_lookup_gtk4.c
 * PURPOSE: Expose logical widget lookup to full and standalone GTK components.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "widget_lookup_internal.h"
/* Lookup inspects ownership but never activates, reveals or focuses a control. */
/* A fixture may inspect an unpresented panel without activating it. Borrow
 * the unique result from the caller-owned tree; traversal itself retains a
 * reference only long enough to detect ambiguity safely. */
void *umi_gtk4_automation_find_tagged_widget(void *native_root, const char *automation_id)
{
    if (native_root == NULL || !GTK_IS_WIDGET(native_root) ||
        automation_id == NULL || automation_id[0] == '\0') return NULL;
    AutomationSearch search = {0};
    automation_find_widgets(GTK_WIDGET(native_root), automation_id, NULL, 0U, &search);
    GtkWidget *result = search.status == UMI_STATUS_OK && search.matches == 1U
        ? search.found : NULL;
    g_clear_object(&search.found);
    return result;
}

/* Compatibility lookup shares the same bounds and logical-child traversal as
 * tagged controls. It cannot broaden the driver to unrelated top-level windows. */
void *umi_gtk4_automation_find_named_widget(void *native_root, const char *widget_name)
{
    if (native_root == NULL || !GTK_IS_WIDGET(native_root) ||
        widget_name == NULL || widget_name[0] == '\0') return NULL;
    AutomationSearch search = {0};
    search.by_widget_name = true;
    automation_find_widgets(GTK_WIDGET(native_root), widget_name, NULL, 0U, &search);
    GtkWidget *result = search.status == UMI_STATUS_OK && search.matches == 1U
        ? search.found : NULL;
    g_clear_object(&search.found);
    return result;
}


/* Capture the logical ownership route before changing UI state. Native parents
 * alone cannot lead out of a collapsed GtkExpander. Retain each route object
 * because selection notifications may synchronously rebuild or close a view. */
typedef struct AutomationRevealRoute {
    GtkWidget *nodes[257];
    unsigned count;
    size_t visited;
    UmiStatus status;
} AutomationRevealRoute;

static bool automation_reveal_route(GtkWidget *widget, GtkWidget *target,
    unsigned depth, AutomationRevealRoute *route)
{
    if (widget == NULL || route->status != UMI_STATUS_OK) return false;
    if (depth >= G_N_ELEMENTS(route->nodes) || route->visited >= 16384U) {
        route->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return false;
    }
    ++route->visited;
    bool found = widget == target;
    if (found) route->count = depth + 1U;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); !found && child != NULL;
         child = gtk_widget_get_next_sibling(child))
        found = automation_reveal_route(child, target, depth + 1U, route);
    if (!found && GTK_IS_EXPANDER(widget)) {
        GtkWidget *content = gtk_expander_get_child(GTK_EXPANDER(widget));
        if (content != NULL && !gtk_widget_is_ancestor(content, widget))
            found = automation_reveal_route(content, target, depth + 1U, route);
    }
    if (found) route->nodes[depth] = g_object_ref(widget);
    return found;
}

/* Revealing is an explicit operation, separate from the read-only lookup.
 * It selects existing pages and expands existing sections without emitting an
 * action, enabling a control, presenting a window or entering the event loop. */
UmiStatus umi_gtk4_automation_reveal_tagged_widget(void *native_root, const char *automation_id)
{
    if (native_root == NULL || !GTK_IS_WIDGET(native_root) ||
        automation_id == NULL || automation_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    AutomationSearch search = {0};
    automation_find_widgets(GTK_WIDGET(native_root), automation_id, NULL, 0U, &search);
    UmiStatus status = search.status;
    if (status == UMI_STATUS_OK && search.matches != 1U)
        status = search.matches == 0U ? UMI_STATUS_NOT_FOUND : UMI_STATUS_ALREADY_EXISTS;
    AutomationRevealRoute route = {0};
    if (status == UMI_STATUS_OK &&
        !automation_reveal_route(GTK_WIDGET(native_root), search.found, 0U, &route))
        status = route.status != UMI_STATUS_OK ? route.status : UMI_STATUS_NOT_FOUND;
    for (unsigned i = 0U; status == UMI_STATUS_OK && i + 1U < route.count; ++i) {
        GtkWidget *owner = route.nodes[i], *next = route.nodes[i + 1U];
        /* Re-resolve after every notification-capable step. A replacement with
         * the same ID is a different action and must never inherit this request. */
        /* A callback can move the same widget into a different section. Its
         * identity alone is insufficient: this captured owner must still own
         * the logical destination before its page state is changed. */
        if (umi_gtk4_automation_find_tagged_widget(native_root, automation_id) != search.found ||
            umi_gtk4_automation_find_tagged_widget(owner, automation_id) != search.found) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        if (GTK_IS_EXPANDER(owner))
            gtk_expander_set_expanded(GTK_EXPANDER(owner), TRUE);
        else if (GTK_IS_STACK(owner) && gtk_widget_get_parent(next) == owner)
            gtk_stack_set_visible_child(GTK_STACK(owner), next);
        else if (GTK_IS_NOTEBOOK(owner)) {
            int pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(owner));
            for (int page = 0; page < pages; ++page) {
                GtkWidget *content = gtk_notebook_get_nth_page(GTK_NOTEBOOK(owner), page);
                if (umi_gtk4_automation_find_tagged_widget(content, automation_id) == search.found) {
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(owner), page);
                    break;
                }
            }
        }
    }
    if (status == UMI_STATUS_OK &&
        umi_gtk4_automation_find_tagged_widget(native_root, automation_id) != search.found)
        status = UMI_STATUS_CANCELLED;
    for (unsigned i = 0U; i < route.count; ++i) g_clear_object(&route.nodes[i]);
    g_clear_object(&search.found);
    return status;
}
