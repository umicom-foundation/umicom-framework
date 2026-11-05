/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/pixverse_gtk4.h
 *
 * PURPOSE:
 *   Expose reviewed video generation and explicit prompt handoff.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
    /* Copy an explicit draft into an idle generator. A nonempty existing
     * draft requires replace_existing=true. Failure preserves all fields;
     * success revokes review/approval and clears the password. No request is
     * sent. Call only on the GTK owner thread with this generator's root. */
    UmiStatus UmiPixVerseGtkSetPrompt(GtkWidget *root, const char *text, bool replace_existing);
#ifdef __cplusplus
}
#endif
#endif
