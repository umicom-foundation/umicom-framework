/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/widget_lookup_internal.h
 * PURPOSE: Share bounded logical GTK traversal without depending on a complete application frontend.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_WIDGET_LOOKUP_INTERNAL_H
#define UMICOM_GTK4_WIDGET_LOOKUP_INTERNAL_H
#include "umicom/base/status.h"
#include <gtk/gtk.h>
#include <string.h>
#ifndef UMI_GTK4_AUTOMATION_ID_KEY
#define UMI_GTK4_AUTOMATION_ID_KEY "umicom-automation-id"
#endif

/* The driver and standalone lookup use one traversal implementation. Keeping
 * this private lets broker inspectors use it without linking the full desktop.
 * Extend logical-container ownership here so both callers retain the same bounds. */
/* Retain the first matching object while checking for ambiguous identifiers.
 * A partial or ambiguous tree is not a licence to activate its first match. */
/* A supported debugger page can contain 1,024 variable rows, each owning
 * labels, entries and collapsed controls. Budget native widgets rather than
 * assuming one widget per record. Search and reveal use the same finite bound. */
#define UMI_GTK4_LOGICAL_TREE_NODE_LIMIT 131072U
#define UMI_GTK4_LOGICAL_TREE_DEPTH_LIMIT 256U

typedef struct AutomationSearch {
    GtkWidget *found;
    size_t matches;
    size_t visited;
    UmiStatus status;
    bool by_widget_name;
} AutomationSearch;
static void automation_find_widgets(GtkWidget *widget, const char *target_id,
    GtkWidget *excluded, unsigned depth, AutomationSearch *search)
{
    GtkWidget *child;
    const char *id;
    if (widget == NULL || widget == excluded || search->status != UMI_STATUS_OK) return;
/* Allow complete supported debugger trees without accepting an ambiguous partial traversal. The former implementation is retained for review. */
#if 0
    if (depth > 256U || search->visited >= 16384U) {
#else
    if (depth > UMI_GTK4_LOGICAL_TREE_DEPTH_LIMIT ||
        search->visited >= UMI_GTK4_LOGICAL_TREE_NODE_LIMIT) {
#endif
        search->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    ++search->visited;
/* The bounded logical traversal now supports existing GTK names as well as automation IDs. One traversal retains ambiguity and ownership checks for both selectors. The previous implementation is retained for engineering review. */
#if 0
    id = g_object_get_data(G_OBJECT(widget), UMI_GTK4_AUTOMATION_ID_KEY);
#endif
    id = search->by_widget_name ? gtk_widget_get_name(widget) :
        g_object_get_data(G_OBJECT(widget), UMI_GTK4_AUTOMATION_ID_KEY);
    if (id != NULL && strcmp(id, target_id) == 0 && search->found != widget) {
        ++search->matches;
        if (search->found == NULL) search->found = g_object_ref(widget);
    }
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        automation_find_widgets(child, target_id, excluded, depth + 1U, search);
    /* GtkExpander owns its content even while the collapsed content is absent
     * from the rendered widget tree. Inspect that logical child once, without
     * expanding it; visible-action checks still require a mapped widget. */
    if (GTK_IS_EXPANDER(widget)) {
        GtkWidget *content = gtk_expander_get_child(GTK_EXPANDER(widget));
        if (content != NULL && !gtk_widget_is_ancestor(content, widget))
            automation_find_widgets(content, target_id, excluded, depth + 1U, search);
    }
}


#endif
