/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/components/window_gtk4.c
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
#include "umicom/ui/gtk4/window.h"
GtkWidget *umi_gtk4_window_new(const UmiUiComponentSpec*s){GtkWidget*w=gtk_window_new();if(s&&s->text[0])gtk_window_set_title(GTK_WINDOW(w),s->text);if(s&&s->width>0&&s->height>0)gtk_window_set_default_size(GTK_WINDOW(w),s->width,s->height);return w;}
GtkWidget *umi_gtk4_header_bar_new(const UmiUiComponentSpec*s){(void)s;return gtk_header_bar_new();}
