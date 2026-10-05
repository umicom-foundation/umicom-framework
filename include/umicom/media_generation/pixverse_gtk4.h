/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/pixverse_gtk4.h
 * PURPOSE: Compose a shared creative workflow in a native application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_PIXVERSE_GTK4_H
#define UMICOM_MEDIA_GENERATION_PIXVERSE_GTK4_H
#include <gtk/gtk.h>
#include "umicom/media_generation/pixverse.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* GTK thread only. Returns a floating widget with its own private state.
 * Construction performs no I/O. Worker jobs own their inputs and use a weak
 * root reference; retained controls cannot call a destroyed panel. */
    UmiStatus UmiPixVerseGtkCreate(const char *application, const char *profile, GtkWidget **out);
#ifdef __cplusplus
}
#endif
#endif
