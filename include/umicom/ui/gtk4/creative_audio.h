/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/creative_audio.h
 * PURPOSE: Present shared creative audio controls through GTK.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A memory-only clip panel with explicit read and new-file export operations. */
#ifndef UMICOM_UI_GTK4_CREATIVE_AUDIO_H
#define UMICOM_UI_GTK4_CREATIVE_AUDIO_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/audio.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Returns a floating GTK widget. Parent it or ref_sink/unref it. All calls and
 * callbacks belong to the GTK owner thread. Creation performs no file I/O.
 * Signal closures disconnect when their root dies; drawing owns copied peaks.
 * Imported clips and previews are not part of a saved creative project. */
GtkWidget *UmiCreativeAudioGtkCreate(void);
/* Copies bytes on success and replaces only this panel's previous clip. An
 * invalid input leaves the previous clip/selection intact. No implicit file I/O. */
UmiStatus UmiCreativeAudioGtkLoadBytes(GtkWidget *root, const void *bytes, size_t length);
#ifdef __cplusplus
}
#endif
#endif
