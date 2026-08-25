/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/breadcrumb.h
 *
 * PURPOSE:
 *   Render semantic breadcrumb navigation for project, symbol, media and application contexts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_BREADCRUMB_H
#define UMICOM_UI_QT6_BREADCRUMB_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_breadcrumb_descriptor(void);
UmiQt6WidgetHandle umi_qt6_breadcrumb_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
