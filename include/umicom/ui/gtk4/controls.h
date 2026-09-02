/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/controls.h
 *
 * PURPOSE:
 *   Expose reusable constructors for common GTK4 controls used across Umicom products.
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

#ifndef UMICOM_UI_GTK4_CONTROLS_H
#define UMICOM_UI_GTK4_CONTROLS_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
/**
 * Provide the gtk4 button new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_button_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 label new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_label_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 entry new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_entry_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 check button new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_check_button_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 switch new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_switch_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 progress new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_progress_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 spinner new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_spinner_new(const UmiUiComponentSpec *spec);
#endif
