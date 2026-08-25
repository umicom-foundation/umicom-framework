/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/monitor.h
 *
 * PURPOSE:
 *   Expose Qt6 screen/monitor presentation state to Framework-owned window restoration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_MONITOR_H
#define UMICOM_UI_QT6_MONITOR_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_monitor_descriptor(void);
UmiQt6WidgetHandle umi_qt6_monitor_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
