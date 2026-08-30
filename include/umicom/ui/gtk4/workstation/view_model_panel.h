/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/view_model_panel.h
 *
 * PURPOSE:
 *   Render generic Framework UmiUiViewModel properties and command actions as
 *   professional GTK4 workstation content without moving model ownership into GTK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_VIEW_MODEL_PANEL_H
#define UMICOM_UI_GTK4_WORKSTATION_VIEW_MODEL_PANEL_H

#include <gtk/gtk.h>

#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiGtk4ViewModelActionHandler)(
    const char *action_id,
    void *user_data);

GtkWidget *umi_gtk4_view_model_panel_create(
    UmiUiViewModel *view,
    UmiGtk4ViewModelActionHandler action_handler,
    void *user_data);

#ifdef __cplusplus
}
#endif
#endif
