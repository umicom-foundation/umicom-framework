/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/views.h
 *
 * PURPOSE:
 *   Render general Framework view components in a Qt6 reusable host.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_VIEWS_H
#define UMICOM_UI_QT6_VIEWS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_views_descriptor(void);
UmiQt6WidgetHandle umi_qt6_views_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
