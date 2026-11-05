/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/seedream_gtk4.h
 *
 * PURPOSE:
 *   Expose reviewed image generation and explicit prompt handoff.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_SEEDREAM_GTK4_H
#define UMICOM_MEDIA_GENERATION_SEEDREAM_GTK4_H
#include <gtk/gtk.h>
#include "umicom/media_generation/seedream.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* GTK thread only. Return a floating widget; construction performs no I/O.
 * Workers independently own requests, images and cancellation tokens. The
 * application/profile namespace is copied and also scopes the local key UI.
 * Generation and PNG export are separate explicit actions. Unsaved results
 * live only for this panel's lifetime; closing cannot cancel remote billing. */
    UmiStatus UmiSeedreamGtkCreate(const char *application, const char *profile, GtkWidget **out);
    /* Copy an explicit draft into an idle generator. A nonempty existing
     * draft requires replace_existing=true. Failure preserves all fields;
     * success revokes review/approval and clears the password. No request is
     * sent. Call only on the GTK owner thread with this generator's root. */
    UmiStatus UmiSeedreamGtkSetPrompt(GtkWidget *root, const char *text, bool replace_existing);
#ifdef __cplusplus
}
#endif
#endif
