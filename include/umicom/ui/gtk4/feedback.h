/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/feedback.h
 *
 * PURPOSE:
 *   Expose reusable GTK4 feedback/state controls.
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

#ifndef UMICOM_UI_GTK4_FEEDBACK_H
#define UMICOM_UI_GTK4_FEEDBACK_H
#include <gtk/gtk.h>
#include "umicom/ui/components/component.h"
/**
 * Provide the gtk4 status message new operation used by this module and its client
 * applications.
 */
GtkWidget *umi_gtk4_status_message_new(const UmiUiComponentSpec *spec);
#endif
