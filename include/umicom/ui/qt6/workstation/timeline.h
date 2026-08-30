/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/timeline.h
 *
 * PURPOSE:
 *   Render multi-track timelines for video, music, animation and replay applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_TIMELINE_H
#define UMICOM_UI_QT6_WORKSTATION_TIMELINE_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/timeline_model.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_timeline_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_timeline_create(const UmiWsTimelineModel *timeline);

#ifdef __cplusplus
}
#endif
#endif
