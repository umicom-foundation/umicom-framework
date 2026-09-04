/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/panel_frame.h
 *
 * PURPOSE:
 *   Render Framework panel chrome around arbitrary GTK4 child content without moving panel ownership into the adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_PANEL_FRAME_H
#define UMICOM_UI_GTK4_WORKSTATION_PANEL_FRAME_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/panel_chrome.h"

/**
 * Initialise gtk4 ws panel frame from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_panel_frame_create(const UmiWsPanelChrome *chrome, GtkWidget *child);
typedef void (*UmiGtk4WsPanelActionHandler)(
    UmiWsPanelAction action,
    const UmiWsPanelChrome *chrome,
    void *user_data);
/**
 * Provide the gtk4 ws panel frame create interactive operation used by this module and its
 * client applications.
 */
GtkWidget *umi_gtk4_ws_panel_frame_create_interactive(
    const UmiWsPanelChrome *chrome,
    GtkWidget *child,
    UmiGtk4WsPanelActionHandler action_handler,
    void *user_data);
/**
 * Apply the current linked-context colour to an existing Framework panel frame.
 */
void umi_gtk4_ws_panel_frame_set_context_colour(
    GtkWidget *frame,
    const char *colour_token);

#endif
