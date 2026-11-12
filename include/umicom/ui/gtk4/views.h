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

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_GTK4_VIEWS_H
#define UMICOM_UI_GTK4_VIEWS_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the gtk4 text view new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_text_view_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 list new operation used by this module and its client applications.
 */
GtkWidget *umi_gtk4_list_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 search entry new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_search_entry_new(const UmiUiComponentSpec *spec);
/**
 * Provide the gtk4 drawing surface new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_drawing_surface_new(const UmiUiComponentSpec *spec);

#ifdef __cplusplus
}
#endif
#endif
