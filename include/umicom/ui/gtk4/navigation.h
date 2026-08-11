/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/navigation.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 navigation and tab/stack constructors.
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

#ifndef UMICOM_UI_GTK4_NAVIGATION_H
#define UMICOM_UI_GTK4_NAVIGATION_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
GtkWidget *umi_gtk4_tab_host_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_stack_new(const UmiUiComponentSpec *spec);
GtkWidget *umi_gtk4_stack_switcher_new(const UmiUiComponentSpec *spec);
#endif
