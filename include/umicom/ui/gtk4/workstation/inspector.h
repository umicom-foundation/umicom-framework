/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/inspector.h
 *
 * PURPOSE:
 *   Render generic property/inspector models as editable/read-only GTK4 property rows for designers and operational panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_INSPECTOR_H
#define UMICOM_UI_GTK4_WORKSTATION_INSPECTOR_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/inspector_model.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws inspector from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_inspector_create(const UmiWsInspectorModel *model);


#ifdef __cplusplus
}
#endif
#endif
