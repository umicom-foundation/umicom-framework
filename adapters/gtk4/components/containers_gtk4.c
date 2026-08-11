/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/containers_gtk4.c
 *
 * PURPOSE:
 *   Implement reusable GTK4 rendering for common semantic Umicom components.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/gtk4/containers.h"
GtkWidget *umi_gtk4_box_new(const UmiUiComponentSpec*s){GtkOrientation o=(s&&s->orientation==UMI_UI_VERTICAL)?GTK_ORIENTATION_VERTICAL:GTK_ORIENTATION_HORIZONTAL;return gtk_box_new(o,s?s->spacing:0);}
GtkWidget *umi_gtk4_grid_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_grid_new();if(s){gtk_grid_set_row_spacing(GTK_GRID(w),(guint)s->spacing);gtk_grid_set_column_spacing(GTK_GRID(w),(guint)s->spacing);}return w;}
GtkWidget *umi_gtk4_paned_new(const UmiUiComponentSpec*s){GtkOrientation o=(s&&s->orientation==UMI_UI_VERTICAL)?GTK_ORIENTATION_VERTICAL:GTK_ORIENTATION_HORIZONTAL;return gtk_paned_new(o);}
GtkWidget *umi_gtk4_scrolled_new(const UmiUiComponentSpec*s){(void)s;return gtk_scrolled_window_new();}
GtkWidget *umi_gtk4_frame_new(const UmiUiComponentSpec*s){return gtk_frame_new(s&&s->text[0]?s->text:NULL);}
GtkWidget *umi_gtk4_expander_new(const UmiUiComponentSpec*s){return gtk_expander_new(s&&s->text[0]?s->text:NULL);}
GtkWidget *umi_gtk4_overlay_new(const UmiUiComponentSpec*s){(void)s;return gtk_overlay_new();}
