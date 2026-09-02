/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/containers.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 layout/container constructors.
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

#ifndef UMICOM_UI_GTK4_CONTAINERS_H
#define UMICOM_UI_GTK4_CONTAINERS_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
/**
 * Provide the gtk4 box new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_box_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 grid new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_grid_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 paned new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_paned_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 scrolled new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_scrolled_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 frame new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_frame_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 expander new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_expander_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 overlay new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_overlay_new(const UmiUiComponentSpec *spec);
#endif
