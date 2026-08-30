/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/window.h
 *
 * PURPOSE:
 *   Expose reusable application window and header-bar constructors.
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

#ifndef UMICOM_UI_GTK4_WINDOW_H
#define UMICOM_UI_GTK4_WINDOW_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
GtkWidget *umi_gtk4_window_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_header_bar_new(const UmiUiComponentSpec *spec);
#endif
