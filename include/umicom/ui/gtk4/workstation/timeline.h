/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/timeline.h
 *
 * PURPOSE:
 *   Render reusable multi-track timeline state for video editing, music production, animation and replay workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_TIMELINE_H
#define UMICOM_UI_GTK4_WORKSTATION_TIMELINE_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/timeline_model.h"

GtkWidget *umi_gtk4_ws_timeline_create(const UmiWsTimelineModel *timeline);

#endif
