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

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws status strip from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_status_strip_create(const UmiWsStatusStrip *strip);


#ifdef __cplusplus
}
#endif
#endif
