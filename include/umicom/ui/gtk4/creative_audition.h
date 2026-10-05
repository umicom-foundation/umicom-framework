/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/creative_audition.h
 * PURPOSE: Audition owned PCM previews with explicit playback and native lifecycle handling.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CREATIVE_AUDITION_H
#define UMICOM_UI_GTK4_CREATIVE_AUDITION_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/audio.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiCreativeAuditionState
    {
        bool loaded, prepared, playing, seekable, backend_error;
        uint64_t source_frames;
        uint32_t sample_rate;
        uint16_t channels;
        double volume;
    } UmiCreativeAuditionState;
    /* Returns a floating widget. Parent it or ref_sink/unref it. All calls and
 * signals belong to the GTK owner thread. Controls use the installed GTK
 * media backend; loading is not proof that an audio device is available.
 * No file is written, URL opened or external player launched. Hiding the
 * widget pauses and detaches its controls; showing it does not resume sound. */
    GtkWidget *UmiCreativeAuditionGtkCreate(void);
    /* Validate with the shared PCM reader and strip optional metadata through a
 * unity render before giving owned bytes to GTK. Input may be freed on return.
 * The existing 4 MiB PCM16 mono/stereo WAVE limit applies. A failure preserves
 * the previous preview. Success replaces it, paused at 20 percent volume.
 * Loading/copying is synchronous bounded CPU work; backend preparation follows
 * GTK asynchronously. Reentrant loads return BUSY. No playback starts here. */
    UmiStatus UmiCreativeAuditionGtkLoadWave(GtkWidget *widget, const void *bytes, size_t length);
    /* Pause and release the preview, including from an edit-invalidation callback.
 * During publication this request is deferred until the current load returns.
 * Retained child controls are detached when their owning widget is disposed. */
    void UmiCreativeAuditionGtkClear(GtkWidget *widget);
    UmiStatus UmiCreativeAuditionGtkRead(GtkWidget *widget, UmiCreativeAuditionState *out);
#ifdef __cplusplus
}
#endif
#endif
