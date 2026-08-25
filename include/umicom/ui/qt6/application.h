/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/application.h
 *
 * PURPOSE:
 *   Bind Framework application presentation state to a Qt6 application-facing host surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_APPLICATION_H
#define UMICOM_UI_QT6_APPLICATION_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_application_descriptor(void);
UmiQt6WidgetHandle umi_qt6_application_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
