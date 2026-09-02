/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/status_strip.h
 *
 * PURPOSE:
 *   Render semantic status-strip items with left/right alignment for IDE, trading, media and operations workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_STATUS_STRIP_H
#define UMICOM_UI_GTK4_WORKSTATION_STATUS_STRIP_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/status_strip.h"

/**
 * Initialise gtk4 ws status strip from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_status_strip_create(const UmiWsStatusStrip *strip);

#endif
