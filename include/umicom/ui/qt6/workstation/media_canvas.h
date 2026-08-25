/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/media_canvas.h
 *
 * PURPOSE:
 *   Render media viewport, waveform and zoomable design-canvas surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_MEDIA_CANVAS_H
#define UMICOM_UI_QT6_WORKSTATION_MEDIA_CANVAS_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/media_viewport.h"
#include "umicom/ui/workstation/waveform_surface.h"
#include "umicom/ui/workstation/canvas_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_media_canvas_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_media_viewport_create(const UmiWsMediaViewport *viewport);
UmiQt6WidgetHandle umi_qt6_ws_waveform_create(const UmiWsWaveformSurface *waveform);
UmiQt6WidgetHandle umi_qt6_ws_canvas_create(const UmiWsCanvasSurface *canvas);

#ifdef __cplusplus
}
#endif
#endif
