/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/dialog.h
 *
 * PURPOSE:
 *   Render Framework dialog requests using a Qt6 dialog host boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_DIALOG_H
#define UMICOM_UI_QT6_DIALOG_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_dialog_descriptor(void);
UmiQt6WidgetHandle umi_qt6_dialog_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
