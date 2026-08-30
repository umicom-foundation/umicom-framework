/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/containers.h
 *
 * PURPOSE:
 *   Render canonical container components as Qt6 box/split/tab compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTAINERS_H
#define UMICOM_UI_QT6_CONTAINERS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_containers_descriptor(void);
UmiQt6WidgetHandle umi_qt6_containers_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
