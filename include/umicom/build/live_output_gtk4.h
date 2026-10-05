/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/live_output_gtk4.h
 * PURPOSE: Present copied live build output without borrowing worker or application services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_LIVE_OUTPUT_GTK4_H
#define UMICOM_BUILD_LIVE_OUTPUT_GTK4_H
#include <gtk/gtk.h>
#include "umicom/build/live_output.h"
#ifdef __cplusplus
extern "C" {
#endif
/* GTK-owner-thread API. The returned floating widget owns its display and one
 * bounded snapshot. Its controls never call build services: hosts poll their
 * session and pass copied output to Update. Follow, Refresh and Copy therefore
 * remain safe when a host closes or a child control is retained separately. */
GtkWidget *UmiBuildOutputGtk4Create(void);
/* Copy valid evidence. A stale operation/revision is rejected without changing
 * the display. Pause stops text replacement but still accepts the newest copy.
 * Copy takes exactly the displayed text, not newer output waiting in memory.
 * NUL bytes appear as U+2400; invalid UTF-8 is replaced for display only.
 * This is a bounded transcript viewer, not a terminal or full build log file. */
UmiStatus UmiBuildOutputGtk4Update(GtkWidget *panel, const UmiBuildOutputSnapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
