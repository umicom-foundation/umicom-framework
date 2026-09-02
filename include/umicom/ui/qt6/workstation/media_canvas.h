/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/media_canvas.h
 *
 * PURPOSE:
 *   Render media viewport, waveform and zoomable design-canvas surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Provide the qt6 ws media canvas descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_media_canvas_descriptor(void);
/**
 * Initialise qt6 ws media viewport from caller-provided values so later operations receive
 * a known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_media_viewport_create(const UmiWsMediaViewport *viewport);
/**
 * Initialise qt6 ws waveform from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_waveform_create(const UmiWsWaveformSurface *waveform);
/**
 * Initialise qt6 ws canvas from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_ws_canvas_create(const UmiWsCanvasSurface *canvas);

#ifdef __cplusplus
}
#endif
#endif
