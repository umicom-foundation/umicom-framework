/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_picker.h
 *
 * PURPOSE:
 *   Render context-link group selection and direction controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_PICKER_H
#define UMICOM_UI_QT6_CONTEXT_PICKER_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_picker_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_picker_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
