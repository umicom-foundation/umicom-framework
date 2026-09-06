/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/window_fit.h
 *
 * PURPOSE:
 *   Provide one small, shared rule for choosing a sensible native workstation
 *   size on different monitors. Product applications use this contract instead
 *   of each copying a different hard-coded window size.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_WINDOW_FIT_H
#define UMICOM_UI_GTK4_WORKSTATION_WINDOW_FIT_H

#include <gtk/gtk.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Choose and apply a monitor-aware default size for a resizable workstation.
 *
 * Use the window's surface monitor, then the pointer's surface monitor, then
 * the first monitor reported by the display. Geometry uses logical pixels and
 * reserves an 80-pixel margin per dimension. Minimum dimensions are preferences,
 * not widget size requests: available monitor space wins on small displays.
 * With no monitor, use the preferred size. All dimensions must be positive.
 * This changes only the initial size and enables normal resizing; the window
 * manager controls placement and the user can still maximise or move it.
 */
UmiStatus umi_gtk4_ws_window_fit(
    GtkWindow *window,
    int preferred_width,
    int preferred_height,
    int minimum_width,
    int minimum_height);

#ifdef __cplusplus
}
#endif

#endif
