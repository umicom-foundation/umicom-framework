/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/selection_list.h
 * PURPOSE: Publish explicit dropdown choices without silently selecting a business record.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_SELECTION_LIST_H
#define UMICOM_UI_GTK4_SELECTION_LIST_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"

/**
 * @brief Publish a fresh string list with an explicit unselected prompt.
 * @param picker The caller-owned dropdown on its GTK owning thread.
 * @param rows A fresh caller-owned list; this function appends one prompt.
 * @param selected A data-row index, or GTK_INVALID_LIST_POSITION for no choice.
 * @return OK, or INVALID_ARGUMENT for reused/invalid objects and
 * CAPACITY_EXCEEDED when the extra prompt cannot be represented.
 *
 * GTK may automatically select the first row of a nonempty model. A prompt at
 * the end keeps data indexes stable while making "no choice" a real native row.
 * Notifications are frozen until the model and selection agree. The dropdown
 * retains the model; the caller still releases its own list reference.
 */
static inline UmiStatus UmiGtk4SelectionListPublish(
    GtkDropDown *picker, GtkStringList *rows, guint selected)
{
    const char *key = "umicom-selection-list-data-count";
    if (!GTK_IS_DROP_DOWN(picker) || !GTK_IS_STRING_LIST(rows) ||
        g_object_get_data(G_OBJECT(rows), key) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    guint count = g_list_model_get_n_items(G_LIST_MODEL(rows));
    if (count == G_MAXUINT) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Model notifications can call application code. Hold both objects until
     * publication finishes, and mark the list before any such notification so
     * a reentrant caller cannot publish the same list twice. */
    g_object_ref(picker);
    g_object_ref(rows);
    g_object_set_data(G_OBJECT(rows), key, GUINT_TO_POINTER(count + 1U));
    if (count != 0U)
        gtk_string_list_append(rows, "Choose an item");
    g_object_freeze_notify(G_OBJECT(picker));
    gtk_drop_down_set_model(picker, count != 0U ? G_LIST_MODEL(rows) : NULL);
    gtk_drop_down_set_selected(picker, count == 0U ? GTK_INVALID_LIST_POSITION
        : selected < count ? selected : count);
    g_object_thaw_notify(G_OBJECT(picker));
    g_object_unref(rows);
    g_object_unref(picker);
    return UMI_STATUS_OK;
}

/**
 * @brief Read a data-row index while treating the prompt as no selection.
 * @param picker A live dropdown on its GTK owning thread.
 * @return A data-row index or GTK_INVALID_LIST_POSITION. Ordinary dropdowns
 * without a selection-list marker retain GTK's normal index semantics.
 *
 * Callers must still validate the index against their captured business data.
 * A selected prompt never grants authority to preview, approve or submit it.
 */
static inline guint UmiGtk4SelectionListSelected(GtkDropDown *picker)
{
    if (!GTK_IS_DROP_DOWN(picker)) return GTK_INVALID_LIST_POSITION;
    guint selected = gtk_drop_down_get_selected(picker);
    GListModel *model = gtk_drop_down_get_model(picker);
    gpointer marker = model != NULL
        ? g_object_get_data(G_OBJECT(model), "umicom-selection-list-data-count") : NULL;
    if (marker != NULL && selected >= GPOINTER_TO_UINT(marker) - 1U)
        return GTK_INVALID_LIST_POSITION;
    return selected;
}
#endif
