/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workbench.h
 *
 * PURPOSE:
 *   Render the canonical Umicom workbench aggregate as a Qt6 main-window composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKBENCH_H
#define UMICOM_UI_QT6_WORKBENCH_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_workbench_descriptor(void);
UmiQt6WidgetHandle umi_qt6_workbench_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
