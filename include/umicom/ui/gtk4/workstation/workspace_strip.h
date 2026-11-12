/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/workspace_strip.h
 *
 * PURPOSE:
 *   Render the saved-layout/workspace strip used for rapid perspective/layout switching across workstation applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_STRIP_H
#define UMICOM_UI_GTK4_WORKSTATION_WORKSPACE_STRIP_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/workspace_strip.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws workspace strip from caller-provided values so later operations
 * receive a known state.
 */
GtkWidget *umi_gtk4_ws_workspace_strip_create(const UmiWsWorkspaceStrip *strip);


#ifdef __cplusplus
}
#endif
#endif
