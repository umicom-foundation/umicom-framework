/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/audio_arrangement.h
 * PURPOSE: Expose shared native audio arrangement editing and explicit render/export actions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_AUDIO_ARRANGEMENT_H
#define UMICOM_UI_GTK4_AUDIO_ARRANGEMENT_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/audio_arrangement.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Floating widget. Creation performs no I/O. Load a saved asset library, edit
 * placements, render and inspect the result, then explicitly export a new WAV.
 * Every accepted plan/library change retires a previous render. File and mix
 * operations run on workers; destroying the widget cancels pending work. */
    GtkWidget *UmiCreativeAudioArrangementGtkCreate(void);
    UmiStatus UmiCreativeAudioArrangementGtkSnapshot(GtkWidget *root, UmiCreativeAudioArrangement *out,
                                                     bool *out_rendered);
#ifdef __cplusplus
}
#endif
#endif
