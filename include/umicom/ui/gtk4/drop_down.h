/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/drop_down.h
 *
 * PURPOSE:
 *   Define GTK dropdown helpers that make model ownership explicit. Umicom
 *   applications use this boundary instead of repeating GTK reference-counting
 *   rules in every panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DROP_DOWN_H
#define UMICOM_UI_GTK4_DROP_DOWN_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a dropdown that takes ownership of the caller's GtkStringList
 * reference. The caller must not unreference items after a successful call.
 * Returning NULL for a NULL list keeps invalid models away from GTK internals.
 */
GtkWidget *umi_ui_gtk4_drop_down_new_take_string_list(GtkStringList *items);

#ifdef __cplusplus
}
#endif

#endif
