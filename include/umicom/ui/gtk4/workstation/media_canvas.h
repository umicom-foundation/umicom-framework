/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/media_canvas.h
 *
 * PURPOSE:
 *   Render shared media/canvas/waveform viewport scaffolds for video, image, music, AI creator and design applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_MEDIA_CANVAS_H
#define UMICOM_UI_GTK4_WORKSTATION_MEDIA_CANVAS_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/media_viewport.h"
#include "umicom/ui/workstation/waveform_surface.h"
#include "umicom/ui/workstation/canvas_surface.h"

GtkWidget *umi_gtk4_ws_media_viewport_create(const UmiWsMediaViewport *viewport);
GtkWidget *umi_gtk4_ws_waveform_create(const UmiWsWaveformSurface *waveform);
GtkWidget *umi_gtk4_ws_canvas_create(const UmiWsCanvasSurface *canvas);

#endif
