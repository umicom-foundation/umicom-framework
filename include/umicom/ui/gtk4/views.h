/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/views.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 text, search, list and drawing surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_GTK4_VIEWS_H
#define UMICOM_UI_GTK4_VIEWS_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
GtkWidget *umi_gtk4_text_view_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_list_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_search_entry_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_drawing_surface_new(const UmiUiComponentSpec *spec);
#endif
