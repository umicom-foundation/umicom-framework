/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/drop_down_gtk4.c
 *
 * PURPOSE:
 *   Implement the shared GTK dropdown ownership boundary used by suite,
 *   appearance and specialist application panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/drop_down.h"

/* Give GTK the model's full reference and return the widget that now owns it. */
GtkWidget *umi_ui_gtk4_drop_down_new_take_string_list(GtkStringList *items)
{
    if (items == NULL) {
        return NULL;
    }

    /* gtk_drop_down_new() consumes the full model reference.  Returning the
     * widget without unreferencing items prevents the dropdown's internal
     * list view from retaining a model that has already been finalised. */
    return gtk_drop_down_new(G_LIST_MODEL(items), NULL);
}
