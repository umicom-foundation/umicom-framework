/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/views_gtk4.c
 *
 * PURPOSE:
 *   Implement reusable GTK4 rendering for common semantic Umicom components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/gtk4/views.h"
/*
 * Provide the gtk4 text view new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_text_view_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_text_view_new();/* Apply this branch only when its contract condition is satisfied. */ if(s&&s->text[0])gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)),s->text,-1);return w;}
/* Provide the gtk4 list new operation used by this module and its client applications. */
GtkWidget *umi_gtk4_list_new(const UmiUiComponentSpec*s){(void)s;return gtk_list_box_new();}
/*
 * Provide the gtk4 search entry new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_search_entry_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_search_entry_new();/* Apply this branch only when its contract condition is satisfied. */ if(s&&s->text[0])gtk_editable_set_text(GTK_EDITABLE(w),s->text);return w;}
/*
 * Provide the gtk4 drawing surface new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_drawing_surface_new(const UmiUiComponentSpec*s){(void)s;return gtk_drawing_area_new();}
