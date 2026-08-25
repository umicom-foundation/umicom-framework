/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/inspector.h
 *
 * PURPOSE:
 *   Render generic property/inspector models as editable/read-only GTK4 property rows for designers and operational panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_INSPECTOR_H
#define UMICOM_UI_GTK4_WORKSTATION_INSPECTOR_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/inspector_model.h"

GtkWidget *umi_gtk4_ws_inspector_create(const UmiWsInspectorModel *model);

#endif
