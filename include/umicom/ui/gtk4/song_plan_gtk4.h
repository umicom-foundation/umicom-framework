/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/song_plan_gtk4.h
 * PURPOSE: Compose a shared creative workflow in a native application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_SONG_PLAN_GTK4_H
#define UMICOM_UI_GTK4_SONG_PLAN_GTK4_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/song_plan.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* GTK thread only. Returns a floating widget with its own private state.
 * Construction performs no I/O. Worker jobs own their inputs and use a weak
 * root reference; retained controls cannot call a destroyed panel. */
    UmiStatus UmiSongPlanGtkCreate(GtkWidget **out);
#ifdef __cplusplus
}
#endif
#endif
