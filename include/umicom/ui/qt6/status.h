/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/status.h
 *
 * PURPOSE:
 *   Render canonical status information and operational modes through Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_STATUS_H
#define UMICOM_UI_QT6_STATUS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_status_descriptor(void);
UmiQt6WidgetHandle umi_qt6_status_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
