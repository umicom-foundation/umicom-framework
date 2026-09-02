/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/navigation.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 navigation and tab/stack constructors.
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

#ifndef UMICOM_UI_GTK4_NAVIGATION_H
#define UMICOM_UI_GTK4_NAVIGATION_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
/**
 * Provide the gtk4 tab host new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_tab_host_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 stack new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_stack_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 stack switcher new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_stack_switcher_new(const UmiUiComponentSpec *spec);
#endif
